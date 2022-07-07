/**
   @brief: common api and variables for fuzzer and gtest
   @file_name: common.hpp
   @Copyright (C) 2022 Pretlist - All Rights Reserved
 */
#ifndef WORKSPACE_TEST_COMMON_HPP
#define WORKSPACE_TEST_COMMON_HPP
#include <gflags/gflags.h>
#include <string>
#include <log/logger.hpp>

#include "IMessagingServiceAdapter.hpp"
#include "IMessagingServiceAdapterMock.hpp"
#include "NSQAdapter.hpp"

#include <google/protobuf/util/time_util.h>
#include "ack_nack.pb.h"
#include "register_app.pb.h"
#include "common.pb.h"
#include "flow_meter.pb.h"

#include "sensor_flow_meter.hpp"
#include "sensor_flow_meter_flags.hpp"
#endif  // WORKSPACE_TEST_COMMON_HPP
