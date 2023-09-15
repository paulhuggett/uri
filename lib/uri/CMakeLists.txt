#===- lib/uri/CMakeLists.txt ----------------------------------------------===//
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
set (include_dir "${URI_ROOT}/include")
add_library (uri STATIC
    "${include_dir}/uri/pctdecode.hpp"
    "${include_dir}/uri/rule.hpp"
    "${include_dir}/uri/uri.hpp"
    rule.cpp
    uri.cpp
)
setup_target (uri)
target_include_directories (
  uri PUBLIC $<BUILD_INTERFACE:${include_dir}> $<INSTALL_INTERFACE:uri>
)
