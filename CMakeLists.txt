cmake_minimum_required(VERSION 3.10)
project(dvtdit)

set(CMAKE_CXX_STANDARD 17)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add source files
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(dvtdit ${SOURCES})
