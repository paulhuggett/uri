#===- CMakeLists.txt ------------------------------------------------------===//
#*   ____ __  __       _        _     _     _        *
#*  / ___|  \/  | __ _| | _____| |   (_)___| |_ ___  *
#* | |   | |\/| |/ _` | |/ / _ \ |   | / __| __/ __| *
#* | |___| |  | | (_| |   <  __/ |___| \__ \ |_\__ \ *
#*  \____|_|  |_|\__,_|_|\_\___|_____|_|___/\__|___/ *
#*                                                   *
#===----------------------------------------------------------------------===//
# Distributed under the MIT License.
# See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
# SPDX-License-Identifier: MIT
#===----------------------------------------------------------------------===//
cmake_minimum_required (VERSION 3.19)
project (uri CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option (URI_FUZZTEST "Enable FuzzTest")
option (URI_LIBCXX "Use libc++ rather than libstdc++")
option (URI_WERROR "Compiler warnings are errors")

if (URI_LIBCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif (URI_LIBCXX)

include (setup_target)
include (setup_gtest)

set (URI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

if (URI_FUZZTEST)
  set (FUZZTEST_FUZZING_MODE On)
  include (FetchContent)
  set (FUZZTEST_REPO_BRANCH "1635d42" CACHE STRING "FuzzTest repository branch.")
  message ("Building fuzztest at tag " ${FUZZTEST_REPO_BRANCH})
  FetchContent_Declare (
    fuzztest
    GIT_REPOSITORY https://github.com/google/fuzztest.git
    GIT_TAG ${FUZZTEST_REPO_BRANCH}
  )
  FetchContent_MakeAvailable (fuzztest)
  include (GoogleTest)
  fuzztest_setup_fuzzing_flags ()
else ()
  setup_gtest ()
endif (URI_FUZZTEST)
enable_testing ()

add_subdirectory (lib)
if (TARGET gtest)
  add_subdirectory (unittests)
endif ()

add_subdirectory (tools)
