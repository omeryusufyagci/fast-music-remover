if(BUILD_TESTING)
    add_executable(ConfigManagerTester ConfigManagerTester.cpp "../src/ConfigManager.cpp" "../src/HardwareUtils.cpp" "../src/Utils.cpp")
    target_link_libraries(ConfigManagerTester gtest_main)
    add_test(NAME ConfigManagerTesting COMMAND ConfigManagerTester)

endif()