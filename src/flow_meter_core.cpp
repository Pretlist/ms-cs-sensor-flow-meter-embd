/*
 * @copyright (C) 2022 Pretlist
 */
#include "flow_meter_core.hpp"

namespace ms_cs_sensor_flow_meter_embd {
FlowMeterCore::FlowMeterCore() {
  m_flow = 6.0F; //for test only
}

FlowMeterCore::~FlowMeterCore() {
}

float FlowMeterCore::GetFlow() {
  return m_flow;
}
}  // namespace ms_cs_sensor_flow_meter_embd
