add_subdirectory(${THIRD_PARTY_DIR}/ms-cs-schema-embd/src ${CMAKE_BINARY_DIR}/schema_build)

find_package(Protobuf REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(folly CONFIG REQUIRED)

add_library(flow_meter
  sensor_flow_meter.cpp 
  sensor_flow_meter_flags.cpp
  flow_meter_core.cpp 
  timer.cpp 
  mean.cpp 
)
target_link_libraries(
  flow_meter 
  ${Protobuf_LIBRARIES}
  schema
  nsqadapter
  gonsq
  CURL::libcurl
  gflags logger spdlog pthread Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util
  rt
)
target_include_directories(
  flow_meter
  PRIVATE
  ${PROJECT_BINARY_DIR}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}
  ${Protobuf_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/schema_build
)

add_executable(ms_sensor_flow_meter ms_sensor_flow_meter.cpp)
target_include_directories(
  ms_sensor_flow_meter
  PRIVATE
  ${PROJECT_BINARY_DIR}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(
  ms_sensor_flow_meter
  flow_meter
)

set_target_properties(flow_meter PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

# install libs
install(TARGETS flow_meter
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}/
        PUBLIC_HEADER DESTINATION include/flow_meter/)
