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
    ${CMAKE_SOURCE_DIR}/src/DeepFilterCommandBuilder.cpp
)

# Link DeepFilter wrt platform
if(APPLE)
    if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
        set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.dylib)
    else()
        message(FATAL_ERROR "x86_64 MacOS is currently not supported (should come very soon!).")
        # set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.dylib)
    endif()
elseif(WIN32)
    set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.dll.a) # .dll.a for linking, .dll for runtime
elseif(UNIX)
    set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.so)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Link the executable with necessary libraries
target_link_libraries(MediaProcessor PRIVATE
    Threads::Threads
    ${SNDFILE_LIBRARIES}
    ${DF_LIBRARY}
)

set_target_properties(MediaProcessor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
