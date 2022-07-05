/*
 * @file ms_sensor_flow_meter.cpp
 * @brief flow_meter sensor microservice
 * @copyright (C) 2022 Pretlist
 */
#include <atomic>
#include <csignal>
#include <iostream>

#include "sensor_flow_meter.hpp"
#include "sensor_flow_meter_flags.hpp"
#include "NSQAdapter.hpp"

static std::atomic<bool> isRunning = true;  // NOLINT (cppcoreguidelines-avoid-non-const-global-variables)
                                            // @name Aravindh
                                            // @date 04/26/22
                                            // @reason fix the clang-tidy-error of non-const and globally accessible

static void SignalHandler(int signal_number) {
  (void)(signal_number);
  isRunning = false;
}
constexpr uint32_t SleepTime = 1;
int main(int argc, char *argv[]) {
  gflags::SetUsageMessage("Pressure sensor microservice");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);
  
  // Create Calibration class object 
  ms_nsq_client::NSQAdapter nsq_pub_sub;
  ms_cs_sensor_flow_meter_embd::SensorFlowMeter sensor(&nsq_pub_sub);
  if (sensor.Scan() == 0) {
    return 0;
  }
  sensor.InitNsq();
  sensor.RegisterApp();
 
  while (isRunning) {
    sleep(10);
  }

  return 0;
}
