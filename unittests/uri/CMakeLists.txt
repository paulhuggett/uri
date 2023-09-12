#===- unittests/uri/CMakeLists.txt ----------------------------------------===//
#*   ____ __  __       _        _     _     _        *
#*  / ___|  \/  | __ _| | _____| |   (_)___| |_ ___  *
#* | |   | |\/| |/ _` | |/ / _ \ |   | / __| __/ __| *
#* | |___| |  | | (_| |   <  __/ |___| \__ \ |_\__ \ *
#*  \____|_|  |_|\__,_|_|\_\___|_____|_|___/\__|___/ *
#*                                                   *
#===----------------------------------------------------------------------===//
#
# Distributed under the MIT License.
# See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
# for license information.
# SPDX-License-Identifier: MIT
#
#===----------------------------------------------------------------------===//
add_executable (unittest
  test_pct_decode.cpp
  test_rule.cpp
  test_uri.cpp
)
target_link_libraries (unittest uri gmock_main)
setup_target (unittest)
set (clang_options -Wno-global-constructors -Wno-used-but-marked-unused)
set (gcc_options)
set (msvc_options -wd4702) # unreachable code.
target_compile_options (
  unittest
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${clang_options}>
    $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
    $<$<CXX_COMPILER_ID:MSVC>:${msvc_options}>
)
