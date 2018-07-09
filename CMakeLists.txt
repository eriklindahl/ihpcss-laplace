#
# Example CMake file for John Urbanic's laplace
# solver, now using a C++ compiler.
#

# Make sure we don't have a pre-historic CMake version
cmake_minimum_required(VERSION 2.8)

# We want a name
project(Laplace)

# Build the binary 'laplace' from the source file (now renamed to be a c++ source)
add_executable(laplace laplace_serial.cpp)

