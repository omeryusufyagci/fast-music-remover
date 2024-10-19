cmake_minimum_required(VERSION 3.10)
project(MediaProcessor)

# C++17 or above is required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Release build by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# Testing build are off by default. can be turned on using cmake -DBUILD_TESTING=ON
option(BUILD_TESTING "Build the tests" OFF)
include(CTest)
# Add subdirectories for source and tests
add_subdirectory(src)
add_subdirectory(tests)