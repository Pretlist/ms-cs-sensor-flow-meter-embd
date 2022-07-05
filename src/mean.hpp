/*
 * @copyright (C) 2022 Pretlist
 */

#ifndef WORKSPACE_SRC_GET_MEAN_HPP
#define WORKSPACE_SRC_GET_MEAN_HPP
#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
namespace mean {
class Mean {
 public:
  Mean();
  ~Mean();
  
  /**
    @brief  Marking copy constructors, assignment operators as NA
   */
  Mean operator=(const Mean &test) = delete;
  Mean(const Mean &test) = delete;
  Mean(Mean &&) = delete;
  Mean &operator=(Mean &&) = delete;

  static float Arithematic(float *arr, uint32_t n);
  static float Geometric(float *arr, uint32_t n);


};
}  // namespace mean
#endif
