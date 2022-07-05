/*
 * @copyright (C) 2022 Pretlist
 */

#ifndef WORKSPACE_SRC_FLOW_METER_CORE_HPP
#define WORKSPACE_SRC_FLOW_METER_CORE_HPP
#include <iostream>

namespace ms_cs_sensor_flow_meter_embd {
class FlowMeterCore {
 public:
  FlowMeterCore();
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
  float m_flow = 0.0F;
};
}  // namespace ms_cs_sensor_flow_meter_embd
#endif
