#===- unittests/uri/CMakeLists.txt ----------------------------------------===//
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
add_executable (unittest
  test_find_last.cpp
  test_parts.cpp
  test_pctdecode.cpp
  test_pctencode.cpp
  test_punycode.cpp
  test_rule.cpp
  test_uri.cpp
)
target_compile_options (
  unittest
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
      -Wno-global-constructors
      -Wno-used-but-marked-unused>
    $<$<CXX_COMPILER_ID:GNU>:>
    # -wd4702 : unreachable code.
    $<$<CXX_COMPILER_ID:MSVC>:
      -wd4702>
)
target_link_libraries (unittest PUBLIC uri)
setup_target (unittest PEDANTIC $<NOT:$<BOOL:${URI_FUZZTEST}>>)
add_test(NAME unittest COMMAND unittest)

if (URI_FUZZTEST)
  target_link_libraries (unittest PUBLIC gtest gmock)
  link_fuzztest (unittest)
  gtest_discover_tests (unittest)
else ()
  target_link_libraries (unittest PUBLIC gmock_main)
endif (URI_FUZZTEST)
