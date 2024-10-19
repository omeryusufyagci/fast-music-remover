cmake_minimum_required(VERSION 3.10)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann)

# Add the source files for the MediaProcessor executable
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

# Link with threads and DeepFilter library
target_link_libraries(MediaProcessor PRIVATE Threads::Threads)
target_link_libraries(MediaProcessor PRIVATE ${CMAKE_SOURCE_DIR}/lib/libdf.so)

# Link with SNDFILE
target_link_libraries(MediaProcessor PRIVATE ${SNDFILE_LIBRARIES})

set_target_properties(MediaProcessor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)