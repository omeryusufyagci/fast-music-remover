# CMake configuration for building the main MediaProcessor executable

add_executable(MediaProcessor
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/AudioProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp
    ${CMAKE_SOURCE_DIR}/src/HardwareUtils.cpp
    ${CMAKE_SOURCE_DIR}/src/FFmpegSettingsManager.cpp
)

target_link_libraries(MediaProcessor PRIVATE Threads::Threads)
target_link_libraries(MediaProcessor PRIVATE ${CMAKE_SOURCE_DIR}/lib/libdf.so)
target_link_libraries(MediaProcessor PRIVATE ${SNDFILE_LIBRARIES})

set_target_properties(MediaProcessor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
