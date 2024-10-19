cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann)

add_executable(MediaProcessor
    main.cpp
    ConfigManager.cpp
    AudioProcessor.cpp
    VideoProcessor.cpp
    Utils.cpp
    CommandBuilder.cpp
    HardwareUtils.cpp
    FFmpegSettingsManager.cpp
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

set_target_properties(MediaProcessor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)