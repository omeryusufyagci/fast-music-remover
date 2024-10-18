if(BUILD_TESTING)
    include(FetchContent)
    cmake_policy(SET CMP0135 NEW)  # Set policy for FetchContent

    find_package(GTest QUIET)
    if(NOT GTEST_FOUND)
        # Fetch GTest
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )

        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()
    
    # setup test media directory
    set(TEST_MEDIA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TestMedia" CACHE PATH "Path to test media files")

    # ConfigManagerTester
    add_executable(ConfigManagerTester 
        ConfigManagerTester.cpp 
        "../src/ConfigManager.cpp" 
        "../src/HardwareUtils.cpp" 
        "../src/Utils.cpp"
        "ConfigFileCreator.cpp"
    )
    target_link_libraries(ConfigManagerTester gtest_main)
    add_test(NAME ConfigManagerTesting COMMAND ConfigManagerTester)

    # UtilsTester
    add_executable(UtilsTester 
        UtilsTester.cpp
        "../src/Utils.cpp"
    )
    target_link_libraries(UtilsTester gtest_main)
    add_test(NAME UtilsTesting COMMAND UtilsTester)

    # AudioProcessorTester
    add_executable(AudioProcessorTester
        AudioProcessorTester.cpp
        "../src/AudioProcessor.cpp"
        "../src/CommandBuilder.cpp"
        "../src/Utils.cpp"
        "../src/HardwareUtils.cpp"
        "../src/ConfigManager.cpp"
        "ConfigFileCreator.cpp"
    )
    target_compile_definitions(AudioProcessorTester PRIVATE TEST_MEDIA_DIR="${TEST_MEDIA_DIR}")
    target_link_libraries(AudioProcessorTester gtest_main)
    add_test(NAME AudioProcessorTesting COMMAND AudioProcessorTester)

    # VideoProcessorTester
    add_executable(VideoProcessorTester
        VideoProcessorTester.cpp
        "../src/VideoProcessor.cpp"
        "../src/CommandBuilder.cpp"
        "../src/Utils.cpp"
        "../src/ConfigManager.cpp"
        "../src/HardwareUtils.cpp"
        "ConfigFileCreator.cpp"
    )
    target_compile_definitions(VideoProcessorTester PRIVATE TEST_MEDIA_DIR="${TEST_MEDIA_DIR}")
    target_link_libraries(VideoProcessorTester gtest_main)
    add_test(NAME VideoProcessorTesting COMMAND VideoProcessorTester)

    # CommandBuilderTester
    add_executable(CommandBuilderTester
        CommandBuilderTester.cpp
        "../src/CommandBuilder.cpp"
        "../src/Utils.cpp"
    )
    target_link_libraries(CommandBuilderTester gtest_main)
    add_test(NAME CommandBuilderTesting COMMAND CommandBuilderTester)

endif()