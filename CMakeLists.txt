# stop if cmake version below 3.21
cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# Define CMAKE_TOOLCHAIN_FILE before setting project
if(DEFINED ENV{PKGMGR_HOME_DIR})
	set(CMAKE_TOOLCHAIN_FILE "$ENV{PKGMGR_HOME_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

if(DEFINED ENV{TARGETPLATFORM})
	if("$ENV{TARGETPLATFORM}" STREQUAL "linux/arm64")
		set(VCPKG_TARGET_TRIPLET "arm64-linux")
		set(triple aarch64-linux-gnueabihf)
		set(CMAKE_C_COMPILER_TARGET ${triple})
		set(CMAKE_CXX_COMPILER_TARGET ${triple})
	endif()
endif()

# project name and supported languages
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} C CXX)
message(STATUS "Project Name : ${ProjectId} (C and CXX)")

# CMake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# guard against in-source builds and bad build-type strings
include(cmake/safeguards.cmake)

# Include Diagnostic flags
include(cmake/diagnostic.cmake)
# Include Diagnostic flags
include(cmake/diagnostic_format.cmake)
# Include linker cmake flags
include(cmake/linker_flags.cmake)
# Include Build Type flags : DEBUG RELEASE EXPERIMENTAL
include(cmake/buildtypes.cmake)
# Include Sanitizers flags : Address Memory Thread CFI Undifined Interger Nullability Implicit_Conversion
# NOTE - SHOULD BE AFTER ALL COMPILER AND LINKER OPTIONS
include(cmake/sanitizers.cmake)

# Generate CMake Args and cmake functions for external/thirdparty repo build
include(cmake/external.cmake)

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
# CMake Option Position Independent Code for all the targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CLANG_MIN_VERSION "12.0.0")
set(BOOST_MIN_VERSION "1.74")


if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

# FIX for CMAKE not setting clang ar/linker/nm/objdump/ranlib
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    SET (CMAKE_AR      "/usr/local/bin/llvm-ar")
    SET (CMAKE_LINKER  "/usr/local/bin/llvm-ld")
    SET (CMAKE_NM      "/usr/local/bin/llvm-nm")
    SET (CMAKE_OBJDUMP "/usr/local/bin/llvm-objdump")
    SET (CMAKE_RANLIB  "/usr/local/bin/llvm-ranlib")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# CHECK LIB PATH
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
message(STATUS "LIB SUFFIX                    " ${LIB64})
if ("${LIB64}" STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()
set(INSTALL_LIB_DIR     lib${LIBSUFFIX} CACHE PATH "Installation directory for libraries")
mark_as_advanced(INSTALL_LIB_DIR)
message(STATUS "INSTALL_LIB_DIR               " ${INSTALL_LIB_DIR})

if(DEFINED ENV{PKGMGR_HOME_DIR})
    if(DEFINED ENV{TARGETPLATFORM})
        if("$ENV{TARGETPLATFORM}" STREQUAL "linux/arm64")
            list(APPEND CMAKE_MODULE_PATH "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/arm64-linux/share/")
            list(APPEND CMAKE_PREFIX_PATH "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/arm64-linux/share/")
        else() # X86_64
            list(APPEND CMAKE_MODULE_PATH "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/share/")
            list(APPEND CMAKE_PREFIX_PATH "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/share/")
        endif()
    else() # X86_64
        list(APPEND CMAKE_MODULE_PATH "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/share/")
        list(APPEND CMAKE_PREFIX_PATH "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/share/")
    endif()
    list(APPEND CMAKE_MODULE_PATH "$ENV{PKGMGR_HOME_DIR}/collection/lib/cmake/")
    list(APPEND CMAKE_MODULE_PATH "$ENV{PKGMGR_HOME_DIR}/collection/lib64/cmake/")
    list(APPEND CMAKE_MODULE_PATH "$ENV{PKGMGR_HOME_DIR}/dsp/lib/cmake/")

    list(APPEND CMAKE_PREFIX_PATH "$ENV{PKGMGR_HOME_DIR}/collection/lib/cmake/")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{PKGMGR_HOME_DIR}/collection/lib64/cmake/")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{PKGMGR_HOME_DIR}/dsp/lib/cmake/")
else()
    message(WARNING "${Yellow}PKGMGR_HOME_DIR is not set, \
                     unable to set MODULE PATH and PREFIX PATH ${ColourReset}")
Endif()

file(GLOB LIBS libs/CMakeLists.txt src/CMakeLists.txt)
file(READ ${LIBS} CONTENT)
if("${CONTENT}" MATCHES "project\\(collection\\)")
    message(WARNING "no need to find collection while building lib-collection")
    if(EXISTS "${PROJECT_SOURCE_DIR}/package.yaml")
         message(WARNING "${Blue}Found package.yaml, processing ... ${ColourReset}")
         execute_process(COMMAND package-installer --task resolve package.yaml
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         RESULT_VARIABLE PKG_DEP_RESULT)
    endif()
else()
    find_package(collection QUIET)
    find_package(Python3 QUIET)
    IF (collection_FOUND)
        message("Collection FOUND Version ${collection_VERSION}")
        if(Python3_FOUND)
            execute_process(COMMAND python3
                            ${PROJECT_SOURCE_DIR}/cmake/check_latest_version.py "lib-collection" ${collection_VERSION}
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE LATEST_REL_RESULT)
            if(EXISTS "${PROJECT_SOURCE_DIR}/package.yaml")
                 message(WARNING "${Blue}Found package.yaml, processing ... ${ColourReset}")
                 execute_process(COMMAND package-installer --task resolve package.yaml
                                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                                 RESULT_VARIABLE PKG_DEP_RESULT)
            endif()
            if(NOT LATEST_REL_RESULT EQUAL 0)
                message(FATAL_ERROR "Older version of libcollection found ${collection_VERSION}, please install latest version ${LATEST_REL_RESULT}")
            endif()
        else()
            message(FATAL_ERROR "Python3 Not found")
        endif()
    else()
        message(WARNING "${Yellow}lib-collection not found, please install latest version${ColourReset}")
    endif()
endif()

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

IF(CMAKE_BUILD_TYPE MATCHES Experimental)
    message(STATUS "Experimental Build")
    set(CMAKE_C_STANDARD 17)
endif()

# CXX Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

# Helpful option enable build profiling to identify slowly compiling files
option(MEASURE_ALL "When enabled all commands will be passed through time command" OFF)
if(MEASURE_ALL)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "time")
endif()

#------------------------------------------------------------------------------
# Set CMAKE FLAG PTHREAD
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
set(HAVE_PTHREAD "${CMAKE_USE_PTHREADS_INIT}")
message(STATUS "THREAD_PREFER_PTHREAD         " ${CMAKE_THREAD_PREFER_PTHREAD})
message(STATUS "PREFER_PTHREAD_FLAG           " ${THREADS_PREFER_PTHREAD_FLAG})
message(STATUS "HAVE_PTHREAD                  " ${CMAKE_USE_PTHREADS_INIT})

#------------------------------------------------------------------------------
# Be nice and export compile commands by default, this is handy for clang-tidy
# and for other tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "EXPORT_COMPILE_COMMANDS  " ${CMAKE_EXPORT_COMPILE_COMMANDS})
#------------------------------------------------------------------------------

option(GTEST_FLAG "If enabled, compile/find the googletest." DISABLE)
option(ENABLE_GFLAG "If enabled, compile/find the GFLags." OFF)
option(ENABLE_BENCHMARK "If enabled, compile/find the Benchmark." OFF)
option(ENABLE_GMOCK "If enabled, compile/find the GTest/GMock." OFF)
option(ENABLE_GTEST "If enabled, compile/find the GTest." ON)
option(WITH_HARDENING "Enable hardening" OFF)
option(ENABLE_COVERAGE "If enabled, compile/find Code Coverage." OFF)
option(ENABLE_EXTCMAKE "If enabled, compile ExternalCMake dep." OFF) # OFF: Recommended thirdparty submodule

message(STATUS "ENABLE GFLAG                  " ${ENABLE_GFLAG})
message(STATUS "ENABLE BENCH                  " ${ENABLE_BENCHMARK})
message(STATUS "ENABLE GMOC                   " ${ENABLE_GMOCK})
message(STATUS "ENABLE GTEST                  " ${ENABLE_GMOCK})
message(STATUS "ENABLE COVERAGE               " ${ENABLE_COVERAGE})
message(STATUS "ENABLE EXTCMAKE               " ${ENABLE_EXTCMAKE})

message(STATUS "OSS FUZZER                    " ${OSS_FUZZER})
message(STATUS "FUZZING TIME                  " ${CMAKE_FUZZING_TIME})

FILE(GLOB_RECURSE  openClSources src/*.cl)
if(openClSources)
    message(STATUS "OpenCL Kernel files found   " ${openClSources})
    list(APPEND BUILD_TYPE_DEBUG "-cl-std=cl2.0")
    list(APPEND BUILD_TYPE_RELEASE "-cl-std=cl2.0")
    list(APPEND BUILD_TYPE_EXPERIMENTAL "-cl-std=cl2.0")
endif()

if(openClSources)
    message(STATUS "OpenCL Kernel files found   " ${openClSources})
    list(APPEND BUILD_TYPE_DEBUG "-cl-std=cl2.0")
    list(APPEND BUILD_TYPE_RELEASE "-cl-std=cl2.0")
    list(APPEND BUILD_TYPE_EXPERIMENTAL "-cl-std=cl2.0")
endif()

# Enable hardening
if (WITH_HARDENING)
    list(APPEND BUILD_TYPE_DEBUG "-fsanitize=safe-stack")
    list(APPEND BUILD_TYPE_RELEASE "-fsanitize=safe-stack")
    list(APPEND BUILD_TYPE_EXPERIMENTAL  "-fsanitize=safe-stack")
endif (WITH_HARDENING)

if (NOT MEMORY_SANITIZER)
    list(APPEND BUILD_TYPE_DEBUG "-stdlib=libstdc++")
    list(APPEND BUILD_TYPE_RELEASE "-stdlib=libstdc++")
    list(APPEND BUILD_TYPE_EXPERIMENTAL  "-stdlib=libstdc++")
endif ()

string (REPLACE ";" " " shared_linker_flags "${shared_linker_flags}")
string (REPLACE ";" " " exe_linker_flags "${exe_linker_flags}")
string (REPLACE ";" " " BUILD_TYPE_DEBUG "${BUILD_TYPE_DEBUG}")
string (REPLACE ";" " " BUILD_TYPE_RELEASE "${BUILD_TYPE_RELEASE}")
string (REPLACE ";" " " BUILD_TYPE_EXPERIMENTAL "${BUILD_TYPE_EXPERIMENTAL}")

message(STATUS "CXX_COMPILER_ID               " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_CXX_COMPILER_VERSION    " ${CMAKE_CXX_COMPILER_VERSION})

# example how to set c++ compiler flags for GNU
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${CLANG_MIN_VERSION})
        if (CMAKE_OVERRIDE_COMPILER_VER)
            message(WARNING  "${Yellow}Insufficient Clang version for runnig will all the flags, please install clang version: ${CLANG_MIN_VERSION} ${ColourReset}")
        else()
            message(FATAL_ERROR  "${Red}Insufficient Clang version for runnig will all the flags, please install clang version: ${CLANG_MIN_VERSION} ${ColourReset}")
        endif()
    endif()
    set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${BUILD_TYPE_EXPERIMENTAL}")
    set(CMAKE_CXX_FLAGS_DEBUG   "${BUILD_TYPE_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${BUILD_TYPE_RELEASE}")
endif()
message(STATUS "BUILD TYPE                    ${CMAKE_BUILD_TYPE}")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "CCACHE Found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# get git hash
include(cmake/git_revision.cmake)

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
message(STATUS "VERSION_MAJOR                 " ${VERSION_MAJOR})
message(STATUS "VERSION_MINOR                 " ${VERSION_MINOR})
message(STATUS "VERSION_PATCH                 " ${VERSION_PATCH})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND ${GTEST_FLAG} STREQUAL DISABLE)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0")
        message(WARNING  "Insufficient Clang version for runnig will all the flags")
    else()
        add_compile_options("${diag_flags}")
        add_compile_options("${diag_format_flags}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${shared_linker_flags}")
        #set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${static_linker_flags}")
    endif()
endif()

# Set linker flag for exe
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${exe_linker_flags}")
#-------------------------------------------------------------------------------
# Set default install location to dist folder in build dir
# we do not want to install to /usr by default
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE )
endif()
#------------------------------------------------------------------------------
if(DEFINED ENV{TARGETPLATFORM})
    if("$ENV{TARGETPLATFORM}" STREQUAL "linux/amd64")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native")
    endif()
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native")
endif()
#------------------------------------------------------------------------------
# Link lib directory install path
if(DEFINED ENV{TARGETPLATFORM})
    if("$ENV{TARGETPLATFORM}" STREQUAL "linux/arm64")
        set(PKGMGR_LIB_DIR "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/arm64-linux/lib/")
        set(PKGMGR_INC_DIR "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/arm64-linux/include/")
    else() # X86_64
        set(PKGMGR_LIB_DIR "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/lib/")
        set(PKGMGR_INC_DIR "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/include/")
    endif()
else() # X86_64
    set(PKGMGR_LIB_DIR "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/lib/")
    set(PKGMGR_INC_DIR "$ENV{PKGMGR_HOME_DIR}/vcpkg/installed/x64-linux/include/")
endif()

set(PKGMGR_COLLECTION_LIB_DIR "$ENV{PKGMGR_HOME_DIR}/collection/lib/")
set(PKGMGR_COLLECTION_LIB64_DIR "$ENV{PKGMGR_HOME_DIR}/collection/lib64/")
set(DSP_LIB_DIR "$ENV{PKGMGR_HOME_DIR}/dsp/lib/")
set(DSP_LIB64_DIR "$ENV{PKGMGR_HOME_DIR}/dsp/lib64/")

set(PKGMGR_COLLECTION_INC_DIR "$ENV{PKGMGR_HOME_DIR}/collection/include/")
set(DSP_INC_DIR "$ENV{PKGMGR_HOME_DIR}/dsp/include/")
link_directories(${CMAKE_INSTALL_PREFIX}/lib
                 ${CMAKE_INSTALL_PREFIX}/lib64
                 ${PKGMGR_LIB_DIR}
                 ${PKGMGR_COLLECTION_LIB_DIR}
                 ${PKGMGR_COLLECTION_LIB64_DIR}
                 ${DSP_LIB_DIR}
                 ${DSP_LIB64_DIR}
)
include_directories(SYSTEM ${CMAKE_INSTALL_PREFIX}/include
                           ${PKGMGR_INC_DIR}
                           ${PKGMGR_COLLECTION_INC_DIR}
                           ${DSP_INC_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)
#------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

# set cdash buildname
set(BUILDNAME
    "${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}-${cmake_build_type_tolower}"
    CACHE STRING
    "Name of build on the dashboard"
    )

if (ENABLE_COVERAGE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    add_compile_options(--coverage)
endif()

# local path for external dependencies resolve CMakeLists.txt
set (_local_dep "${PROJECT_SOURCE_DIR}/cmake/external/CMakeLists.txt")

if (ENABLE_GFLAG)
    # find_package(GFlags MODULE)
    add_subdirectory(third_party/gflags)
endif()

if (ENABLE_BENCHMARK)
    # find_package(GFlags MODULE)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(third_party/benchmark)
    add_subdirectory(benchmarks)
endif()

if (ENABLE_EXTCMAKE)
    if ( EXISTS "${_local_dep}")
        # Add external dependencies
        add_subdirectory(cmake/external)
    endif()
endif()

# local path for external dependencies resolve CMakeLists.txt
set (_thirdparty_dep "${PROJECT_SOURCE_DIR}/third_party/CMakeLists.txt")

if ( EXISTS "${_thirdparty_dep}")
    # Add thirdparty dependencies
    add_subdirectory(third_party)
endif()

# Add project source directory
set (_src_build "${PROJECT_SOURCE_DIR}/src/CMakeLists.txt")
if ( EXISTS "${_src_build}")
    # Add thirdparty dependencies
    add_subdirectory(src)
endif()

# Add project source directory
set (_libs_build "${PROJECT_SOURCE_DIR}/libs/CMakeLists.txt")
if ( EXISTS "${_libs_build}")
    # Add thirdparty dependencies
    add_subdirectory(libs)
endif()

# if(ENABLE_GTEST)
if(${GTEST_FLAG} STREQUAL ENABLE)
    # allow to report to a cdash dashboard
    include(CTest)
    # enable testing
    enable_testing()
    # define tests
    add_subdirectory(test/gtest)
endif()

if (OSS_FUZZER)
    add_subdirectory(test/fuzz)
endif()

# cpack
include(cmake/packaging.cmake)
