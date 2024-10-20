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

# Tests are excluded by default, use `cmake -DBUILD_TESTING=ON` to build with tests.
option(BUILD_TESTING "Test Build" OFF)

# Set include directories
include(CTest)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann)

# Threads is required
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# SNDFILE: for read/write of sampled audio files
# https://github.com/libsndfile/libsndfile
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)
include_directories(${SNDFILE_INCLUDE_DIRS})

# Include the cmake files under cmake/
include(cmake/src.cmake)
if(BUILD_TESTING)
    include(cmake/test.cmake)
endif()
