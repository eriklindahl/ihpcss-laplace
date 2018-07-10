# Build the binary 'laplace' from the source file (now renamed to be a c++ source)
add_executable(laplace laplace_serial.cpp)

install(TARGETS laplace RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
