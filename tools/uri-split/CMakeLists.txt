#===- tools/uri-split/CMakeLists.txt --------------------------------------===//
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
add_executable (uri-split uri-split.cpp)
setup_target (uri-split)
target_link_libraries (uri-split PUBLIC uri)

if (TARGET gtest)
  set (URI_TEST_TARGET unittest)
  set (URI_OUT_XML "${CMAKE_BINARY_DIR}/${test_target}.xml")
  add_custom_command (
    TARGET uri-split
    PRE_LINK
    COMMAND "$<TARGET_FILE:${URI_TEST_TARGET}>"
            "--gtest_output=xml:${URI_OUT_XML}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running unit tests"
    BYPRODUCTS "${URI_OUT_XML}"
    VERBATIM
  )
endif (TARGET gtest)
