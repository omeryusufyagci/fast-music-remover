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

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann)

add_executable(MediaProcessor
    src/main.cpp
    src/ConfigManager.cpp
    src/AudioProcessor.cpp
    src/VideoProcessor.cpp
    src/Utils.cpp
    src/CommandBuilder.cpp
    src/HardwareUtils.cpp
    src/DeepFilterCommandBuilder.cpp
    src/FFmpegSettingsManager.cpp
)

# Threads is required
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(MediaProcessor PRIVATE Threads::Threads)

# Link the DeepFilter library
target_link_libraries(MediaProcessor PRIVATE ${CMAKE_SOURCE_DIR}/lib/libdf.so)

# SNDFILE: for read/write of sampled audio files
# https://github.com/libsndfile/libsndfile
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)
include_directories(${SNDFILE_INCLUDE_DIRS})
target_link_libraries(MediaProcessor PRIVATE ${SNDFILE_LIBRARIES})
