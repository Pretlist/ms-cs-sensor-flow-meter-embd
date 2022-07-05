#include "sensor_flow_meter.hpp"
#include "sensor_flow_meter_flags.hpp"

#include "register_app.pb.h"
#include "sensor_flow_meter.pb.h"
#include "ack_nack.pb.h"
#include <google/protobuf/util/time_util.h>

namespace ms_cs_sensor_flow_meter_embd {
constexpr uint32_t kDefaultTimeoutRead = 1; // seconds
constexpr uint32_t kDefaultTimeoutRes = 10; // seconds
constexpr uint32_t kDefaultTimeoutPub = 300; // seconds

SensorFlowMeter::SensorFlowMeter(ms_nsq_client::IMessagingServiceAdapter* pub_sub): m_nsq_pubsub(pub_sub) {
  m_logger = new Logger("ms_cs_sensor_flow_meter_embd", logger::Type::CONSOLELOG | logger::Type::FILELOG, "ms_cs_sensor_flow_meter_embd_logger.txt");
  m_logger->SetConsoleLevel(static_cast<logger::Level>(FLAGS_ms_cs_sensor_flow_meter_embd_loglevel));
  m_logger->SetFileLevel(static_cast<logger::Level>(FLAGS_ms_cs_sensor_flow_meter_embd_loglevel));

  // Init timer
  m_timer.Register(mTimerCallback, static_cast<void *>(this));
  m_timer.SetTimeout(timer::Timer::Seconds(1));
}

SensorFlowMeter::~SensorFlowMeter() {
  m_timer.Stop();
  m_nsq_pubsub->StopPublisher();
  m_nsq_pubsub->UnsubscribeAll();
  m_thread_nsq_sub->join();
  delete m_thread_nsq_sub;

  // Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  for (auto it : m_map_sensor) {
    delete it.second->flow_meter_core;
    delete it.second;
  }

  delete m_logger;
}

int8_t SensorFlowMeter::Scan() {
  // sensor 1
  m_map_sensor.insert({SENSOR_FLOW_METER_INLET, mCreateSensorCore()});
  
  // sensor 2
  m_map_sensor.insert({SENSOR_FLOW_METER_OUTLET, mCreateSensorCore()});

  // number of sensors scanned
  int8_t num_sensor = static_cast<int8_t>(m_map_sensor.size());
  m_logger->Info("[%s]: Number of scaned sensors = %d", __func__, num_sensor);
  return num_sensor;
}

void SensorFlowMeter::InitNsq() {
  // Create NSQ publisher
  if (!m_nsq_pubsub->CreatePublisher(FLAGS_nsq_ip_port, ms_nsq_client::IMessagingServiceAdapter::E_TCP)) {
    m_logger->Error("[%s]: Failed to create NSQ publisher: %s", __func__, strerror(errno));
  }

  // Create NSQ subscriber 
  mNsqSubTopic(m_topic_cmd);
  m_topic_ack = m_topic_event + "-ack";
  mNsqSubTopic(m_topic_ack);
  m_topic_conf = m_topic_cmd + "-config";
  mNsqSubTopic(m_topic_conf);
  m_thread_nsq_sub = new folly::CPUThreadPoolExecutor(1);
  auto nsq_sub_thread = [&]() { m_nsq_pubsub->ConnectAndStartSubscriber(); };
  m_thread_nsq_sub->add(nsq_sub_thread);
}

void SensorFlowMeter::RegisterApp() {
  folly::fbvector<uint8_t> serialized_pkt;
  for (auto it : m_map_sensor) {
    // Add ID and pub/sub topic as device-metadata
    RegisterApplication::RegisterApp registerData;
    registerData.set_id(it.first);
    registerData.set_ingress_topic(m_topic_event);
    registerData.set_egress_topic(m_topic_cmd);
    *registerData.mutable_time_utc() = google::protobuf::util::TimeUtil::GetCurrentTime();
    // m_logger->Info("---------------------------:%s", google::protobuf::util::TimeUtil::ToString(registerData.time_utc()).c_str());

    // serialization for publisher and submit to pub/sub
    serialized_pkt.resize(registerData.ByteSize());
    registerData.SerializeToArray(serialized_pkt.data(), serialized_pkt.size());
    if (m_nsq_pubsub->PublishMessage(m_topic_register, &serialized_pkt)) {
      m_logger->Info("[%s] Register App request published with pkt id: %x, ingress_topic: %s, egress_topic: %s", __func__, it.first, m_topic_event.c_str(), m_topic_cmd.c_str());
      m_timer.Start();
    } else {
      m_logger->Error("[%s] Failed to publish register data for id = %u", __func__, it.first);
    }
  }
}

void SensorFlowMeter::GenTelmetryData(uint32_t id, struct SensorData *sensor, float sensor_val, Notification notify_type, SensorStatus state) {
  FlowMeter::FlowMeterMsg sensor_msg;
  sensor_msg.set_id(id);
  sensor_msg.set_seq_num(++sensor->counter_seq_num);
  sensor_msg.set_notification(notify_type);
  *sensor_msg.mutable_time_utc() = google::protobuf::util::TimeUtil::GetCurrentTime();
  sensor_msg.set_status(state);
  sensor_msg.set_flow(sensor_val);

  // serialization for publisher and submit to pub/sub
  sensor->serialized_pkt.resize(sensor_msg.ByteSize());
  sensor_msg.SerializeToArray(sensor->serialized_pkt.data(), sensor->serialized_pkt.size());
}

bool SensorFlowMeter::PubTelemetryData(struct SensorData *sensor) {
  sensor->is_published = m_nsq_pubsub->PublishMessage(m_topic_event, &sensor->serialized_pkt);
  if (!sensor->is_published) {
    m_logger->Error("[%s] Failed to publish telemetry data: Seq Number = %u", __func__, sensor->counter_seq_num);
    return sensor->is_published.load();
  }

  // Increment response
  ++sensor->counter_nack;
  if (sensor->counter_nack > kMaxRetry) {
    // TODO take decision
    m_logger->Warn("[%s] Ack not received for Max limit, take a decision", __func__);
  }

  return sensor->is_published.load();
}

SensorStatus SensorFlowMeter::GetCurrState(struct SensorData *sensor, float sensor_val) {
  // find current status
  if (sensor_val < sensor->threshold_min) {
    return SensorStatus::SENSOR_STATUS_LOW; 
  } else if (sensor_val > sensor->threshold_max) {
    return SensorStatus::SENSOR_STATUS_HIGH; 
  } else {
    return SensorStatus::SENSOR_STATUS_GOOD; 
  }
}

// Process message received on NSQ callback
void SensorFlowMeter::mProcessRxMsg(char* msg, uint32_t msg_len, char* topic) {
  m_logger->Info("[%s] rx topic: %s", __func__, topic);
  if (m_topic_cmd.compare(topic) == 0) {
    // not required as this is a sensor
  } else if (m_topic_ack.compare(topic) == 0) {
    AckNack::AckNackMsg res_msg;
    res_msg.ParseFromArray(msg, msg_len); 
    struct SensorData *sensor = nullptr;
    try {
      sensor = m_map_sensor.at(static_cast<SensorMepId>(res_msg.id()));
    } catch (const std::out_of_range &e) {
      m_logger->Warn("[%s] (id = %u) Exception at: %s", __func__, res_msg.id(), e.what());
      return;
    }
    sensor->is_published = false; // reset
    m_logger->Info("[%s] (id = %u) ACK-NACK seq_number: %u, response: %d", __func__, res_msg.id(), res_msg.seq_num(), res_msg.response());
    if (res_msg.response() == AckNack::AckNackMsg::ACK) {
      sensor->counter_nack = 0; // reset
    } else {
      m_logger->Warn("[%s] (id = %u) Nack received. Retrying with previous data", __func__, res_msg.id());
      PubTelemetryData(sensor); // previous data
    }
  } else if (m_topic_conf.compare(topic) == 0) {
    // TODO configuration if required 
  } 
}

void SensorFlowMeter::mTimeout() {
  for (auto it : m_map_sensor) {
    ++it.second->counter_pub;

    // Resend previous sensor data (Telemetry data)
    if (it.second->is_published) {
      ++it.second->counter_res;
      if (it.second->counter_res > it.second->timeout_res) {
        it.second->counter_res = 0; // reset response counter
        m_logger->Warn("[%s] (id = %u) Response timeout. Retrying with previous data", __func__, it.first);
        PubTelemetryData(it.second); // previous data
      }
    } else {
      it.second->counter_res = 0; // reset response counter
    }

    // Time to read sensor data
    if (0 == (it.second->counter_pub % it.second->timeout_read)) {
      mHandleData(it.first, it.second);
    }

    // Publish sensor data (Telemetry data)
    if (it.second->counter_pub == it.second->timeout_pub) {
      m_logger->Info("[%s] (id = %u) Publish current data", __func__, it.first);
      // Get maen of saved sensor data
      float sensor_val_mean = mean::Mean::Arithematic(it.second->sensor_buff.data(), it.second->sensor_buff.size());
      // float sensor_val_mean = mean::Mean::Geometric(it.second->sensor_buff.data(), it.second->sensor_buff.size());
      GenTelmetryData(it.first, it.second, sensor_val_mean, Notification::NOTIFICATION_PERIODIC, GetCurrState(it.second, sensor_val_mean)); // Update sensor data
      PubTelemetryData(it.second); // current data
      it.second->counter_pub = 0; // reset publish counter
      it.second->counter_res = 0; // reset response counter
      it.second->sensor_buff.clear();
    }  
  }
}

struct SensorData* SensorFlowMeter::mCreateSensorCore() {
  auto *sensor = new struct SensorData;
  memset(sensor, 0, sizeof(struct SensorData));
  sensor->threshold_min = 2.0F;
  sensor->threshold_max = 10.0F;
  sensor->timeout_read = kDefaultTimeoutRead;
  sensor->timeout_res = kDefaultTimeoutRes;
  sensor->timeout_pub = kDefaultTimeoutPub;
  sensor->flow_meter_core = new FlowMeterCore();

  return sensor;
}

void SensorFlowMeter::mNsqSubTopic(const std::string& topic) {
  ms_nsq_client::IMessagingServiceAdapter::subscriberOptions sub_options = {
    FLAGS_nsq_ip_port,
    ms_nsq_client::IMessagingServiceAdapter::E_CONNECT_TO_DAEMON,
    ms_nsq_client::IMessagingServiceAdapter::E_TCP,
    topic,
    "ch-ms-cs-sensor-flow-meter-embd",
    static_cast<void *>(this),
    &SensorFlowMeter::mNsqSubCallback};
  m_nsq_pubsub->Subscribe(&sub_options);
}

void SensorFlowMeter::mHandleData(uint32_t id, struct SensorData *sensor) {
  float sensor_val = sensor->flow_meter_core->GetFlow();
  sensor->sensor_buff.push_back(sensor_val); // add data to buffer
  SensorStatus curr_state = GetCurrState(sensor, sensor_val);

  // check if status changed
  if (sensor->prev_state != curr_state) {
    // Change previous state to current
    sensor->prev_state = curr_state;

    // publish alert-notification
    m_logger->Info("[%s] (id = %u) Publish current data (Alert)", __func__, id);
    GenTelmetryData(id, sensor, sensor_val, Notification::NOTIFICATION_ALERT, sensor->prev_state); // Update sensor data
    PubTelemetryData(sensor); // current data (Alert)
  }
}

void SensorFlowMeter::mNsqSubCallback(char* msg, uint32_t msg_len, char* topic, void* context) {
  auto *obj_ptr = static_cast<SensorFlowMeter *>(context);
  if (obj_ptr != nullptr) {
    obj_ptr->mProcessRxMsg(msg, msg_len, topic);
  }
}

void SensorFlowMeter::mTimerCallback(void *userdata) {
  auto *ptr = static_cast<SensorFlowMeter *>(userdata); 
  if (ptr != nullptr) {
    ptr->mTimeout();
  }
}
}  // namespace ms_cs_sensor_flow_meter_embd
