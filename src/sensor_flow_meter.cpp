#include "sensor_flow_meter.hpp"
#include "sensor_flow_meter_flags.hpp"

#include "register_app.pb.h"
#include "flow_meter.pb.h"
#include "sensor_mep_config_schema.pb.h"
#include <google/protobuf/util/time_util.h>

namespace ms_cs_sensor_flow_meter_embd {
constexpr uint32_t kDefaultTimeoutRead = 1; // seconds
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
  // m_map_sensor.insert({Common::SENSOR_FLOW_METER_INLET, mCreateSensorCore()});
  
  // sensor 2
  m_map_sensor.insert({Common::SENSOR_FLOW_METER_OUTLET, mCreateSensorCore()});

  // number of sensors scanned
  int8_t num_sensor = static_cast<int8_t>(m_map_sensor.size());
  m_logger->Debug("[%s]: Number of scaned sensors = %d", __func__, num_sensor);
  return num_sensor;
}

void SensorFlowMeter::InitNsq() {
  // Create NSQ publisher
  if (!m_nsq_pubsub->CreatePublisher(FLAGS_nsq_ip_port, ms_nsq_client::IMessagingServiceAdapter::E_TCP)) {
    m_logger->Error("[%s]: Failed to create NSQ publisher: %s", __func__, strerror(errno));
  }

  // Create NSQ subscriber 
  m_topic_config = m_topic_event + "-config";
  mNsqSubTopic(m_topic_config);

  m_thread_nsq_sub = new folly::CPUThreadPoolExecutor(1);
  auto nsq_sub_thread = [&]() { m_nsq_pubsub->ConnectAndStartSubscriber(); };
  m_thread_nsq_sub->add(nsq_sub_thread);
}

void SensorFlowMeter::RegisterApp() {
  folly::fbvector<uint8_t> serialized_pkt;
  for (auto it : m_map_sensor) {
    // Add ID and pub/sub topic as device-metadata
    RegisterApp::RegisterAppMsg reg_msg;
    reg_msg.set_id(it.first);
    reg_msg.set_ingress_topic(m_topic_event);
    reg_msg.set_egress_topic("");
    *reg_msg.mutable_time_utc() = google::protobuf::util::TimeUtil::GetCurrentTime();
    // m_logger->Info("---------------------------:%s", google::protobuf::util::TimeUtil::ToString(reg_msg.time_utc()).c_str());

    // serialization for publisher and submit to pub/sub
    folly::fbvector<uint8_t> serialized_pkt(reg_msg.ByteSizeLong());
    reg_msg.SerializeToArray(serialized_pkt.data(), serialized_pkt.size());
    if (m_nsq_pubsub->PublishMessage(m_topic_register, &serialized_pkt)) {
      m_logger->Debug("[%s] Register App request published with pkt id: %x, ingress_topic: %s", __func__, it.first, m_topic_event.c_str());
    } else {
      m_logger->Error("[%s] Failed to publish register data for id = %u", __func__, it.first);
    }
  }
}

bool SensorFlowMeter::PubTelemetryData(Common::SensorMepId id, struct SensorData *sensor, float sensor_val, Common::Notification notify_type, Common::SensorStatus state) {
  FlowMeter::FlowMeterMsg sensor_msg;
  sensor_msg.set_id(id);
  sensor_msg.set_seq_num(++sensor->counter_seq_num);
  sensor_msg.set_notification(notify_type);
  *sensor_msg.mutable_time_utc() = google::protobuf::util::TimeUtil::GetCurrentTime();
  sensor_msg.set_status(state);
  sensor_msg.set_flow(sensor_val);

  // serialization for publisher and submit to pub/sub
  folly::fbvector<uint8_t> serialized_pkt(sensor_msg.ByteSizeLong());
  sensor_msg.SerializeToArray(serialized_pkt.data(), serialized_pkt.size());
  if (!m_nsq_pubsub->PublishMessage(m_topic_event, &serialized_pkt)) {
    m_logger->Error("[%s] Failed to publish telemetry data: Seq Number = %u", __func__, sensor->counter_seq_num);
    return false;
  } else {
    m_logger->Debug("[%s] sensor flow data: %f", __func__, sensor_val);
  }

  return true;
}

Common::SensorStatus SensorFlowMeter::GetCurrState(struct SensorData *sensor, float sensor_val) {
  // find current status
  if (sensor_val < sensor->threshold_min) {
    return Common::SENSOR_STATUS_LOW; 
  } else if (sensor_val > sensor->threshold_max) {
    return Common::SENSOR_STATUS_HIGH; 
  } else {
    return Common::SENSOR_STATUS_GOOD; 
  }
}

// Process message received on NSQ callback
void SensorFlowMeter::mProcessRxMsg(char* msg, uint32_t msg_len, char* topic) {
  m_logger->Debug("[%s] rx topic: %s", __func__, topic);
  if (m_topic_config.compare(topic) == 0) {
    mConfigure(msg, msg_len);
  } 
}

void SensorFlowMeter::mConfigure(char* msg, uint32_t msg_len) {
  SensorMepConfigData::SensorAndMepConfig config_msg; 
  config_msg.ParseFromArray(msg, msg_len);

  if (m_map_sensor.find(config_msg.sensor_mep_id()) == m_map_sensor.end()) {
    m_logger->Error("[%s] Invalid sensorId(%u): configuration discarded", __func__, config_msg.sensor_mep_id());
    return;
  }

  struct SensorData *sensor =  m_map_sensor.at(config_msg.sensor_mep_id());
  sensor->threshold_min = config_msg.minthreshold();
  sensor->threshold_max = config_msg.maxthreshold();
  sensor->timeout_pub = config_msg.periodicity();

  m_logger->Debug("[%s]: Threshold Min : %f", __func__, sensor->threshold_min);
  m_logger->Debug("[%s]: Threshold Max : %f", __func__, sensor->threshold_max);
  m_logger->Debug("[%s]: Periodicity   : %u", __func__, sensor->timeout_pub);

  // start timer to read and publish telemetry data
  m_timer.Start();
}

void SensorFlowMeter::mTimeout() {
  for (auto it : m_map_sensor) {
    ++it.second->counter_pub;

    // Time to read sensor data
    if (0 == (it.second->counter_pub % it.second->timeout_read)) {
      mHandleData(it.first, it.second);
    }

    // Publish sensor data (Telemetry data)
    if (it.second->counter_pub == it.second->timeout_pub) {
      m_logger->Debug("[%s] (id = %u) Publish current data", __func__, it.first);
      // Get maen of saved sensor data
      float sensor_val_mean = mean::Mean::Arithematic(it.second->sensor_buff.data(), it.second->sensor_buff.size());
      // float sensor_val_mean = mean::Mean::Geometric(it.second->sensor_buff.data(), it.second->sensor_buff.size());
      PubTelemetryData(it.first, it.second, sensor_val_mean, Common::NOTIFICATION_PERIODIC, GetCurrState(it.second, sensor_val_mean));
      it.second->counter_pub = 0; // reset publish counter
      it.second->sensor_buff.clear();
    }  
  }
}

struct SensorData* SensorFlowMeter::mCreateSensorCore() {
  auto *sensor = new struct SensorData;
  memset(sensor, 0, sizeof(struct SensorData));
  sensor->threshold_min = 0.0F;
  sensor->threshold_max = 0.0F;
  sensor->timeout_read = kDefaultTimeoutRead;
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

void SensorFlowMeter::mHandleData(Common::SensorMepId id, struct SensorData *sensor) {
  float sensor_val = sensor->flow_meter_core->GetFlow();
  sensor->sensor_buff.push_back(sensor_val); // add data to buffer
  Common::SensorStatus curr_state = GetCurrState(sensor, sensor_val);

  // check if status changed
  if (sensor->prev_state != curr_state) {
    // Change previous state to current
    sensor->prev_state = curr_state;

    // publish alert-notification
    m_logger->Debug("[%s] (id = %u) Publish current data (Alert)", __func__, id);
    PubTelemetryData(id, sensor, sensor_val, Common::NOTIFICATION_ALERT, curr_state);
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
