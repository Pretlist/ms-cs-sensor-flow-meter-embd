#include "mean.hpp"
namespace mean {
  Mean::Mean(){}
  Mean::~Mean(){}
  
  float Mean::Geometric(float *arr, uint32_t n){

    // Compute the sum of all the
    // elements in the array.
    float GeoSum = 0;
    for (uint32_t i = 0; i < n; ++i){
      GeoSum += log(arr[i]);
    }

    // compute geometric mean through formula
    // antilog(((log(1) + log(2) + . . . + log(n))/n)
    // and return the value to main function.

    return exp(GeoSum/n);
  }
  
  float Mean::Arithematic(float *arr, uint32_t n){

    // Compute the sum of all the
    // elements in the array.
    float ArithSum = 0;
    for (uint32_t i = 0; i < n; ++i)
      ArithSum += arr[i];

    // compute arithematic mean through formula sum of all numbers in set divided by number of elements

    return (ArithSum/n);
  }
}  // namespace ms_sensor_pressure
