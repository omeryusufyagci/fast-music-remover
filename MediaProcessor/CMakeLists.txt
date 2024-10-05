cmake_minimum_required(VERSION 3.10)
project(MediaProcessor)

# C++17 or above is required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Release build by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann)


add_executable(MediaProcessor
    src/main.cpp
    src/ConfigManager.cpp
    src/AudioProcessor.cpp
    src/VideoProcessor.cpp
    src/Utils.cpp
    src/CommandBuilder.cpp
)

# Threads is required
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(MediaProcessor PRIVATE Threads::Threads)