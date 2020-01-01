project(Algebra)

cmake_minimum_required(VERSION 3.5)

set(default_build_type "Release")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(Catch REQUIRED)

set(SOURCES
    src/Permutation.cpp
    src/Group.cpp
  #  src/Structure.cpp
)

add_library(unit_tests
    tests/test_permutation.cpp
    tests/test_group.cpp
    tests/test_graph.cpp
)

add_subdirectory(tests)
add_subdirectory(examples)
