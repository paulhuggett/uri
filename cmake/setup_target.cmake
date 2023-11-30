#===- cmake/setup_target.cmake --------------------------------------------===//
#*           _                 _                       _    *
#*  ___  ___| |_ _   _ _ __   | |_ __ _ _ __ __ _  ___| |_  *
#* / __|/ _ \ __| | | | '_ \  | __/ _` | '__/ _` |/ _ \ __| *
#* \__ \  __/ |_| |_| | |_) | | || (_| | | | (_| |  __/ |_  *
#* |___/\___|\__|\__,_| .__/   \__\__,_|_|  \__, |\___|\__| *
#*                    |_|                   |___/           *
#===----------------------------------------------------------------------===//
# Distributed under the MIT License.
# See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
# SPDX-License-Identifier: MIT
#===----------------------------------------------------------------------===//
include (CheckCXXCompilerFlag)

# Configure the named target with a standard set of options which enable lots
# of warnings, select the desired language standard, and so on.
function (setup_target target)
  set (
    clang_options
    -Weverything
    -Wno-c++14-extensions
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-c99-extensions
    -Wno-exit-time-destructors
    -Wno-padded
    -Wno-undef
    -Wno-weak-vtables
  )
  set (gcc_options -Wall -Wextra -pedantic -Wno-maybe-uninitialized)
  set (
    msvc_options
    -W4 # enable lots of warnings
    -wd4068 # unknown pragma
    -wd4324 # structure was padded due to alignment specifier
  )

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
    check_cxx_compiler_flag (
      -Wno-return-std-move-in-c++11 CLANG_RETURN_STD_MOVE_IN_CXX11
    )
    if (${CLANG_RETURN_STD_MOVE_IN_CXX11})
      list (APPEND clang_options -Wno-return-std-move-in-c++11)
    endif ()
    check_cxx_compiler_flag (-Wno-c++20-compat CLANG_W_NO_CXX20_COMPAT)
    if (${CLANG_W_NO_CXX20_COMPAT})
      list (APPEND clang_options -Wno-c++20-compat)
    endif ()
    check_cxx_compiler_flag (-Wno-c++2a-compat CLANG_W_NO_CXX2A_COMPAT)
    if (${CLANG_W_NO_CXX2A_COMPAT})
      list (APPEND clang_options -Wno-c++2a-compat)
    endif ()
    check_cxx_compiler_flag (
      -Wno-unsafe-buffer-usage CLANG_W_UNSAFE_BUFFER_USAGE
    )
    if (${CLANG_W_UNSAFE_BUFFER_USAGE})
      list (APPEND clang_options -Wno-unsafe-buffer-usage)
    endif ()
  endif ()

  if (URI_WERROR)
    list (APPEND clang_options -Werror)
    list (APPEND gcc_options -Werror)
    list (APPEND msvc_options /WX)
  endif ()

  if (COVERAGE)
    list (APPEND gcc_options -fprofile-arcs -ftest-coverage)
    list (APPEND clang_options -fprofile-instr-generate -fcoverage-mapping)
  endif ()

  set_target_properties (
    ${target}
    PROPERTIES CXX_STANDARD ${STANDARD}
               CXX_STANDARD_REQUIRED Yes
               CXX_EXTENSIONS No
  )

  set (is_clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>)
  target_compile_options (
    ${target}
    PRIVATE
      $<${is_clang}>:${clang_options}>
      $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
      $<$<CXX_COMPILER_ID:MSVC>:${msvc_options}>
  )
  target_link_options (
    ${target}
    PRIVATE
      $<${is_clang}>:${clang_options}>
      $<$<CXX_COMPILER_ID:GNU>:${gcc_options}>
      $<$<CXX_COMPILER_ID:MSVC>:>
  )
  target_compile_definitions (${target} PUBLIC URI_FUZZTEST=$<BOOL:$URI_FUZZTEST>)

endfunction (setup_target)
