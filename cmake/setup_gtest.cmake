include (CheckCXXCompilerFlag)

# Configure the Google Test targets for our build.
function (setup_gtest)
  if (EXISTS "${URI_ROOT}/googletest/CMakeLists.txt")
    # Tell gtest to link against the "Multi-threaded Debug DLL runtime library"
    # on Windows.
    set (gtest_force_shared_crt On CACHE BOOL "Always use msvcrt.dll")
    # We don't want to install either gtest or gmock.
    set (INSTALL_GTEST Off CACHE BOOL "Disable gtest install")
    set (INSTALL_GMOCK Off CACHE BOOL "Disable gmock install")
    add_subdirectory ("${URI_ROOT}/googletest")

    set (gclangopts )
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
      check_cxx_compiler_flag (
        -Wno-implicit-int-float-conversion
        CLANG_W_NO_IMPLICIT_INT_FLOAT_CONVERSION
      )
      if (${CLANG_W_NO_IMPLICIT_INT_FLOAT_CONVERSION})
        list (APPEND gclangopts -Wno-implicit-int-float-conversion)
      endif ()
    endif ()

    set (is_clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>)

    # Adjust compiler options for the gtest/gmock.
    foreach (target gtest gmock gmock_main gtest_main)
      set_target_properties (
        ${target}
        PROPERTIES CXX_STANDARD ${STANDARD}
                   CXX_STANDARD_REQUIRED Yes
                   CXX_EXTENSIONS No
      )
      target_compile_definitions (
        ${target} PUBLIC GTEST_REMOVE_LEGACY_TEST_CASEAPI_=1
      )
      target_compile_options (${target} PRIVATE $<${is_clang}>:${gclangopts}>)
      target_link_options (${target} PRIVATE $<${is_clang}>:${gclangopts}>)
    endforeach ()
  endif ()

endfunction (setup_gtest)
