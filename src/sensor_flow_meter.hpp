/*
 * @copyright (C) 2022 Pretlist
 */

#ifndef WORKSPACE_SRC_SENSOR_FLOW_METER_HPP
#define WORKSPACE_SRC_SENSOR_FLOW_METER_HPP
#include <iostream>
#include <string>
#include <unordered_map>
#include "log/logger.hpp"
#include <folly/executors/CPUThreadPoolExecutor.h>
#include "flow_meter_core.hpp"
#include "sensor_flow_meter_flags.hpp"
#include "timer.hpp"
#include "mean.hpp"
#include "IMessagingServiceAdapter.hpp"

#include "common.pb.h"

namespace ms_cs_sensor_flow_meter_embd {
struct SensorData {
  uint32_t counter_seq_num;
  uint32_t counter_pub;
  float threshold_min;
  float threshold_max;
  Common::SensorStatus prev_state;
  
  folly::fbvector<float> sensor_buff;
  
  uint32_t timeout_read;
  uint32_t timeout_pub;
  uint32_t timeout_res;
  
  //Core class object
  FlowMeterCore *flow_meter_core;
};

class SensorFlowMeter {
 public:
  SensorFlowMeter(ms_nsq_client::IMessagingServiceAdapter* pub_sub);

  ~SensorFlowMeter();
  
  /**
    @brief  Marking copy constructors, assignment operators as NA
   */
  SensorFlowMeter operator=(const SensorFlowMeter &test) = delete;
  SensorFlowMeter(const SensorFlowMeter &test) = delete;
  SensorFlowMeter(SensorFlowMeter &&) = delete;
  SensorFlowMeter &operator=(SensorFlowMeter &&) = delete;

  int8_t Scan();
  void InitNsq();
  void SetDevLoc(Common::InstallationType dev_loc);
  void RegisterApp();
  bool PubTelemetryData(Common::SensorMepId id, struct SensorData *sensor, float sensor_val, Common::Notification notify_type, Common::SensorStatus state);
  Common::SensorStatus GetCurrState(struct SensorData *sensor, float sensor_val);

 protected:
  void mProcessRxMsg(char* msg, uint32_t msg_len, char* topic);
  void mTimeout();

 private:
  struct SensorData* mCreateSensorCore(Common::SensorMepId id);
  void mNsqSubTopic(const std::string& topic);
  void mHandleData(Common::SensorMepId id, struct SensorData *sensor);
  
  static void mNsqSubCallback(char* msg, uint32_t msg_len, char* topic, void* context);
  static void mTimerCallback(void *userdata);
  void mConfigure(char* msg, uint32_t msg_len);

  Logger *m_logger = nullptr;
  std::unordered_map<Common::SensorMepId, struct SensorData *> m_map_sensor{};

  // NSQ
  ms_nsq_client::IMessagingServiceAdapter* m_nsq_pubsub{};
  folly::CPUThreadPoolExecutor *m_thread_nsq_sub = nullptr;
  std::string m_topic_register = "ms-cs-core-ctrl-sensor-register";
  std::string m_topic_event = "ms-cs-sensor-flow-meter";
  std::string m_topic_config = "";

  // Timer
  timer::Timer m_timer;
};
}  // namespace ms_cs_sensor_flow_meter_embd
#endif
