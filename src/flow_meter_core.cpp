/*
 * @copyright (C) 2022 Pretlist
 */
#include "flow_meter_core.hpp"

namespace ms_cs_sensor_flow_meter_embd {
FlowMeterCore::FlowMeterCore(Common::SensorMepId id) {
  if (id == Common::SensorMepId::SENSOR_FLOW_METER_INLET) {
    m_flow_channel = 1;
  } else { // Common:SensorMepId::SENSOR_FLOW_METER_OUTLET
    m_flow_channel = 0;
  }
}

FlowMeterCore::~FlowMeterCore() {
}

float FlowMeterCore::GetFlow() {
  return 6.0F; // for test only
}
}  // namespace ms_cs_sensor_flow_meter_embd
