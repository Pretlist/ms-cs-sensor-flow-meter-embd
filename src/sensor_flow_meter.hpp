/*
 * @copyright (C) 2022 Pretlist
 */

#ifndef WORKSPACE_SRC_SENSOR_FLOW_METER_HPP
#define WORKSPACE_SRC_SENSOR_FLOW_METER_HPP
#include <iostream>
#include <string>
#include <atomic>
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
constexpr uint8_t kMaxRetry = 3;

struct SensorData {
  uint8_t counter_nack;
  uint32_t counter_seq_num;
  uint32_t counter_pub;
  uint32_t counter_res;
  float threshold_min;
  float threshold_max;
  SensorStatus prev_state;
  
  folly::fbvector<uint8_t> serialized_pkt;
  folly::fbvector<float> sensor_buff;
  
  std::atomic<bool> is_published;
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
  void RegisterApp();
  void GenTelmetryData(uint32_t id, struct SensorData *sensor, float sensor_val, Notification notify_type, SensorStatus state);
  bool PubTelemetryData(struct SensorData *sensor);
  SensorStatus GetCurrState(struct SensorData *sensor, float sensor_val);

 protected:
  void mProcessRxMsg(char* msg, uint32_t msg_len, char* topic);
  void mTimeout();

 private:
  struct SensorData* mCreateSensorCore();
  void mNsqSubTopic(const std::string& topic);
  void mHandleData(uint32_t id, struct SensorData *sensor);
  
  static void mNsqSubCallback(char* msg, uint32_t msg_len, char* topic, void* context);
  static void mTimerCallback(void *userdata);

  Logger *m_logger = nullptr;
  std::unordered_map<SensorMepId, struct SensorData *> m_map_sensor{};

  // NSQ
  ms_nsq_client::IMessagingServiceAdapter* m_nsq_pubsub{};
  folly::CPUThreadPoolExecutor *m_thread_nsq_sub = nullptr;
  std::string m_topic_register = "ms-cs-core-ctrl-sensor-register";
  std::string m_topic_event = "ms-cs-sensor-flow-meter";
  std::string m_topic_cmd = "";
  std::string m_topic_ack = "";
  std::string m_topic_conf = "";

  // Timer
  timer::Timer m_timer;
};
}  // namespace ms_cs_sensor_flow_meter_embd
#endif
