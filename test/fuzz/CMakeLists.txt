set(FUZZ_TARGET "fuzz_test")
find_package(absl)

add_executable(
    ${FUZZ_TARGET}
    validate_fuzz.cc
    ../common.cpp
    )

message("OSS-FUZZ OPT [ADDRESS] ... " ${FUZZ_ADDRESS})
message("OSS-FUZZ OPT  [MEMORY] ... " ${FUZZ_MEMORY})

if (FUZZ_ADDRESS)
    set(FUZZ_FLAGS "-fsanitize=undefined,address,fuzzer")
elseif (FUZZ_MEMORY)
    set(FUZZ_FLAGS "-fsanitize=undefined,memory,fuzzer")
endif()

message("OSS-FUZZ Target Name ... " ${FUZZ_TARGET})
message("OSS-FUZZ Compile Opt ... " ${FUZZ_FLAGS})
message("OSS-FUZZ Fuzz Time   ... " "${CMAKE_FUZZING_TIME}")

set_target_properties(${FUZZ_TARGET} PROPERTIES COMPILE_FLAGS ${FUZZ_FLAGS})

target_link_libraries(
    ${FUZZ_TARGET}
    ${FUZZ_FLAGS}
        zmq
        dsplibproxy
        collection::collection
        absl::time
    )

target_include_directories(
    ${FUZZ_TARGET}
    PRIVATE
        ${PROJECT_BINARY_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/test
        ${CMAKE_CURRENT_LIST_DIR}
    )

add_custom_target("run-fuzz")
add_custom_command(
 TARGET   "run-fuzz"
 COMMAND  "./${FUZZ_TARGET}" -max_total_time="${CMAKE_FUZZING_TIME}"
 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set (destination "${CMAKE_SOURCE_DIR}/")
