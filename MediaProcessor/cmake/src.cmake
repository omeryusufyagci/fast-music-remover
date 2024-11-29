# CMake configuration for building the main MediaProcessor executable

add_executable(MediaProcessor
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/AudioProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/MediaProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp
    ${CMAKE_SOURCE_DIR}/src/HardwareUtils.cpp
    ${CMAKE_SOURCE_DIR}/src/FFmpegSettingsManager.cpp
    ${CMAKE_SOURCE_DIR}/src/DeepFilterCommandBuilder.cpp
)

# Link DeepFilter wrt platform
if(APPLE)
    include(CheckCXXCompilerFlag)

    # This fixes arch detection on macOS
    check_cxx_compiler_flag("-arch arm64" COMPILER_SUPPORTS_ARM64)

    if(COMPILER_SUPPORTS_ARM64 AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.dylib)
    else()
        message(FATAL_ERROR "Unsupported macOS architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
        # set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.dylib)
    endif()
elseif(WIN32)
    set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.dll.a) # .dll.a for linking, .dll for runtime
elseif(UNIX)
    set(DF_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libdf.so)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()


target_link_libraries(MediaProcessor PRIVATE
    Threads::Threads
    ${SNDFILE_LIBRARIES}
    ${DF_LIBRARY}
    nlohmann_json::nlohmann_json
    fmt::fmt
)

target_compile_options(MediaProcessor PRIVATE -D_GLIBCXX_USE_CXX23_ABI)

# Some of this was for macOS try to remove if possible
set_target_properties(MediaProcessor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib"
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INTERPROCEDURAL_OPTIMIZATION TRUE
)
