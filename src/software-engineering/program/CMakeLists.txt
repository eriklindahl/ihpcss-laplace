# Build the binary 'laplace' from the source file (now renamed to be a c++ source)
add_executable(laplace laplace_serial.cpp)
target_link_libraries(laplace ${EXTRA_LIBRARIES})

install(TARGETS laplace RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
