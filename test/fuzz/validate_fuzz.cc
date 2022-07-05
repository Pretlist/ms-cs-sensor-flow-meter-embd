/*
@file_name: validate_fuzz.cc
@author: Aravindh
@brief: Test code for fuzzer
@date: 27/06/2022
@Copyright (C) 2022 Pretlist - All Rights Reserved
*/
#include "common.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  (void)(data);
  if (size > 0) {
  }
  return 0;
}
