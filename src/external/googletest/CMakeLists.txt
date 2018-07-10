
set(GOOGLETEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GOOGLETEST_INCLUDE_DIR})

# EXCLUDE_FROM_ALL means the library is only built when we actually need the tests
add_library(googletest STATIC EXCLUDE_FROM_ALL src/gtest-all.cc src/gtest_main.cc)


