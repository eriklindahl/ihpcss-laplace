
# GTest/GMock suggest linking with pthreads when available for thread safety
set(CMAKE_THREAD_PREFER_PTHREAD 1)
find_package(Threads)
set(PTHREADS_LIBRARIES)
if (CMAKE_USE_PTHREADS_INIT)
    set(PTHREADS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    list(APPEND GOOGLETEST_COMPILE_DEFINITIONS "GTEST_HAS_PTHREAD=1")
    set(GTEST_IS_THREADSAFE 1)
else()
    list(APPEND GOOGLETEST_COMPILE_DEFINITIONS "GTEST_HAS_PTHREAD=0")
    set(GTEST_IS_THREADSAFE 0)
endif()

set(GOOGLETEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GOOGLETEST_INCLUDE_DIR})

# EXCLUDE_FROM_ALL means the library is only built when we actually need the tests
add_library(googletest STATIC EXCLUDE_FROM_ALL src/gtest-all.cc src/gtest_main.cc)
set_property(TARGET googletest APPEND PROPERTY COMPILE_DEFINITIONS "${GOOGLETEST_COMPILE_DEFINITIONS}")
set(GOOGLETEST_LIBRARIES googletest ${PTHREADS_LIBRARIES} PARENT_SCOPE)

