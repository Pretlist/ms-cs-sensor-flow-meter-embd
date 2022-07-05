/**
   @brief: Test GPS NMEA sentences
   @file_name: main.cpp
   @author: skumar
   @date: 22/04/2022
   @Copyright (C) 2022 Pretlist - All Rights Reserved
 */
#include "../common.hpp"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  gflags::SetUsageMessage("Pivot Motor help menu");
  gflags::SetVersionString("1.0.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}
