cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
find_package(absl)

add_executable(
  unit_tests
  main.cpp
  ../common.cpp
  test.cpp
)
target_link_libraries(
  unit_tests
  flow_meter
  collection::collection
  gmock
  absl::time
)
target_include_directories(
  unit_tests
  PRIVATE
  ${PROJECT_BINARY_DIR}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/test
  ${CMAKE_CURRENT_LIST_DIR}
)

add_test(
  NAME
  flowMeterSensorTest
  COMMAND
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
)

add_custom_target("run_gtest")
add_custom_command(
  TARGET   "run_gtest"
  COMMAND  "./unit_tests"
  DEPENDS unit_tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
