/*
  @copyright (C) 2022 Pretlist
 */
#include "sensor_flow_meter_flags.hpp"
DEFINE_uint32(ms_cs_sensor_flow_meter_embd_loglevel, 1, "flow meter sensor receiver logger level");
DEFINE_string(nsq_ip_port, "localhost:4150", "NSQ TCP address <ip:port>");
DEFINE_uint32(installation, 0, "installtion type (0 = unknown, 1 = well, 2 = fertilizer, 3 = anchor, 4 = edge, 5 = midfield)");
