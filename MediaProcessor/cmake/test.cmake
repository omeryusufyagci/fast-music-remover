include(FetchContent)

find_package(GTest QUIET)
if(NOT GTEST_FOUND)
    # Fetch GTest if not found
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Setup test media directory
set(TEST_MEDIA_DIR "${CMAKE_SOURCE_DIR}/tests/TestMedia" CACHE PATH "Path to test media files")

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.0
)
FetchContent_MakeAvailable(fmt)

# Common libraries for all test targets
set(COMMON_LIBRARIES gtest_main ${CMAKE_SOURCE_DIR}/lib/libdf.so ${SNDFILE_LIBRARIES} fmt::fmt)

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

