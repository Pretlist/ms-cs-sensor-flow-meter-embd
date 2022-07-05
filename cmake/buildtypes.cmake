# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

list(APPEND BUILD_TYPE_DEBUG
        "-fwhole-program-vtables"
        "-march=native"
        "-fvectorize"
        "-ftree-vectorize"
        "-pthread"
        "-ffast-math"
        "-O0"
        "-g"
        "-fstandalone-debug"
        "-ftrapv"
        "-fno-omit-frame-pointer"
        "-fno-optimize-sibling-calls"
        "-fno-common"
        "-flto"
        "-ffine-grained-bitfield-accesses"
        "-std=c17"
        "-std=c++17"
        "-std=cuda"
        "-stdlib=libstdc++"
        "-fblocks"
        "-march=native"
        "-mtune=native"
        )

    list(APPEND BUILD_TYPE_RELEASE
        "-Ofast"
        "-fwhole-program-vtables"
        "-march=native"
        "-fvectorize"
        "-ftree-vectorize"
        "-pthread"
        "-ffast-math"
        "-fno-common"
        "-flto"
        "-ffine-grained-bitfield-accesses"
        "-std=c17"
        "-std=c++17"
        "-std=cuda"
        "-stdlib=libstdc++"
        "-fblocks"
        "-march=native"
        "-mtune=native"
        )

list(APPEND BUILD_TYPE_EXPERIMENTAL
        "-fwhole-program-vtables"
        "-Ofast"
        "-march=native"
        "-fvectorize"
        "-ftree-vectorize"
        "-pthread"
        "-pthreads"
        "-ffast-math"
        "-fno-common"
        "-flto"
        "-ffine-grained-bitfield-accesses"
        "-std=c17"
        "-std=c++2a"
        "-std=cuda"
        "-stdlib=libstdc++"
        "-rtlib=compiler-rt"
        "-ffreestanding"
        "-fblocks"
        "-march=native"
        "-mtune=native"
         )
