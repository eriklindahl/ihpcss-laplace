include(CheckCXXCompilerFlag)

# Test C++ flags FLAGS, and set VARIABLE to true if the work. Also add the
# flags to CXXFLAGSVAR if it worked.
macro(test_and_append_cxx_flag VARIABLE FLAGS CXXFLAGSVAR)
    if(NOT DEFINED ${VARIABLE})
        check_cxx_compiler_flag("${FLAGS}" ${VARIABLE})
    endif()
    if(${VARIABLE})
        set(${CXXFLAGSVAR} "${${CXXFLAGSVAR}} ${FLAGS}")
    endif()
endmacro()


# Try to add a flag to enable all warnings. This can be irritating, but it's
# a great way to catch subtle bugs - good code should compile without warnings.
test_and_append_cxx_flag(CXX_ALLWARNINGS "-Wall" CMAKE_CXX_FLAGS)

# Enable fast-math with gcc (can be dangerous for some algorithms!)
test_and_append_cxx_flag(CXX_FAST_MATH "-ffast-math" CMAKE_CXX_FLAGS)

# Try the -fast flag (used with Intel compilers)
# For some reason, clang only warns about this option rather than refusing it,
# so to avoid the warning we only apply it for the intel compiler
# (which gives us a chance to show a compiler-specific command too).
if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    test_and_append_cxx_flag(CXX_FAST "-fast" CMAKE_CXX_FLAGS)
endif()
