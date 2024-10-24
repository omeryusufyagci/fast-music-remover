include(FetchContent)
cmake_policy(SET CMP0135 NEW)  # Use the latest policy for FetchContent consistency

find_package(GTest QUIET)
if(NOT GTEST_FOUND)
    # Fetch GTest if not found
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Setup test media directory
set(TEST_MEDIA_DIR "${CMAKE_SOURCE_DIR}/tests/TestMedia" CACHE PATH "Path to test media files")

# Common libraries for all test targets
set(COMMON_LIBRARIES gtest_main ${CMAKE_SOURCE_DIR}/lib/libdf.so ${SNDFILE_LIBRARIES})

# Macro for adding a test executable
macro(add_test_executable name)
    add_executable(${name} ${ARGN})
    target_compile_definitions(${name} PRIVATE TEST_MEDIA_DIR="${TEST_MEDIA_DIR}")
    target_link_libraries(${name} PRIVATE ${COMMON_LIBRARIES})
    add_test(NAME ${name} COMMAND ${name})
endmacro()

# Add test executables using the macro
add_test_executable(ConfigManagerTester 
    ${CMAKE_SOURCE_DIR}/tests/ConfigManagerTester.cpp 
    ${CMAKE_SOURCE_DIR}/src/ConfigManager.cpp 
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp 
    ${CMAKE_SOURCE_DIR}/src/HardwareUtils.cpp 
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp 
    ${CMAKE_SOURCE_DIR}/tests/TestUtils.cpp
)

add_test_executable(UtilsTester 
    ${CMAKE_SOURCE_DIR}/tests/UtilsTester.cpp 
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp 
)

add_test_executable(AudioProcessorTester
    ${CMAKE_SOURCE_DIR}/tests/AudioProcessorTester.cpp 
    ${CMAKE_SOURCE_DIR}/src/AudioProcessor.cpp 
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp 
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp 
    ${CMAKE_SOURCE_DIR}/src/HardwareUtils.cpp 
    ${CMAKE_SOURCE_DIR}/src/ConfigManager.cpp 
    ${CMAKE_SOURCE_DIR}/tests/TestUtils.cpp
)

add_test_executable(VideoProcessorTester
    ${CMAKE_SOURCE_DIR}/tests/VideoProcessorTester.cpp 
    ${CMAKE_SOURCE_DIR}/src/VideoProcessor.cpp 
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp 
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp 
    ${CMAKE_SOURCE_DIR}/src/ConfigManager.cpp 
    ${CMAKE_SOURCE_DIR}/src/HardwareUtils.cpp 
    ${CMAKE_SOURCE_DIR}/tests/TestUtils.cpp
)

add_test_executable(CommandBuilderTester
    ${CMAKE_SOURCE_DIR}/tests/CommandBuilderTester.cpp 
    ${CMAKE_SOURCE_DIR}/src/CommandBuilder.cpp 
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp
)

