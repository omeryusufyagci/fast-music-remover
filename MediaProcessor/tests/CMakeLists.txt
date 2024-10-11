if(BUILD_TESTING)
    add_executable(ConfigManagerTester 
        ConfigManagerTester.cpp 
        "../src/ConfigManager.cpp" 
        "../src/HardwareUtils.cpp" 
        "../src/Utils.cpp"
        "ConfigFileCreator.cpp"
    )
    target_link_libraries(ConfigManagerTester gtest_main)
    add_test(NAME ConfigManagerTesting COMMAND ConfigManagerTester)

    add_executable(UtilsTester 
        UtilsTester.cpp
        "../src/Utils.cpp"
        "ConfigFileCreator.cpp"
    )
    target_link_libraries(UtilsTester gtest_main)
    add_test(NAME UtilsTesting COMMAND UtilsTester)

endif()