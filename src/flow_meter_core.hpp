/*
 * @copyright (C) 2022 Pretlist
 */

#ifndef WORKSPACE_SRC_FLOW_METER_CORE_HPP
#define WORKSPACE_SRC_FLOW_METER_CORE_HPP
#include <iostream>

#include "common.pb.h"

namespace ms_cs_sensor_flow_meter_embd {
class FlowMeterCore {
 public:
  FlowMeterCore(Common::SensorMepId id);
  ~FlowMeterCore();
  
  /**
    @brief  Marking copy constructors, assignment operators as NA
   */
  FlowMeterCore operator=(const FlowMeterCore &test) = delete;
  FlowMeterCore(const FlowMeterCore &test) = delete;
  FlowMeterCore(FlowMeterCore &&) = delete;
  FlowMeterCore &operator=(FlowMeterCore &&) = delete;

  float GetFlow();

 private:
  int m_flow_channel = 0; // set in constructor based on flow inlet/outlet
};
}  // namespace ms_cs_sensor_flow_meter_embd
#endif
