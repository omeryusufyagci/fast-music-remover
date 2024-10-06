cmake_minimum_required(VERSION 3.10)

add_executable(MediaProcessor
    main.cpp
    ConfigManager.cpp
    AudioProcessor.cpp
    VideoProcessor.cpp
    Utils.cpp
    CommandBuilder.cpp
    HardwareUtils.cpp
)

target_include_directories(MediaProcessor PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/third_party/nlohmann
)

# Threads is required
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(MediaProcessor PRIVATE Threads::Threads)