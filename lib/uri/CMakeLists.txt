#===- lib/uri/CMakeLists.txt ----------------------------------------------===//
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
set (URI_INCLUDE_DIR "${URI_ROOT}/include")
add_library (uri STATIC
    "${URI_INCLUDE_DIR}/uri/find_last.hpp"
    "${URI_INCLUDE_DIR}/uri/icubaby.hpp"
    "${URI_INCLUDE_DIR}/uri/parts.hpp"
    "${URI_INCLUDE_DIR}/uri/pctdecode.hpp"
    "${URI_INCLUDE_DIR}/uri/pctencode.hpp"
    "${URI_INCLUDE_DIR}/uri/punycode.hpp"
    "${URI_INCLUDE_DIR}/uri/rule.hpp"
    "${URI_INCLUDE_DIR}/uri/starts_with.hpp"
    "${URI_INCLUDE_DIR}/uri/uri.hpp"
    parts.cpp
    pctencode.cpp
    punycode.cpp
    rule.cpp
    uri.cpp
)
setup_target (uri)
target_include_directories (
  uri PUBLIC $<BUILD_INTERFACE:${URI_INCLUDE_DIR}> $<INSTALL_INTERFACE:uri>
)
