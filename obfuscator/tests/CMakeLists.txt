# 测试套件

# 简单的测试程序（不依赖外部测试框架）
add_executable(basic_test
    unit/basic_test.cpp
)
target_link_libraries(basic_test PRIVATE obfuscator_core)

# 添加测试
add_test(NAME BasicTest COMMAND basic_test)

# 如果有Google Test，可以添加更多测试
find_package(GTest QUIET)
if(GTEST_FOUND)
    message(STATUS "Google Test found, additional tests will be built")

    add_executable(full_test_suite
        unit/strategy_test.cpp
        unit/engine_test.cpp
    )
    target_link_libraries(full_test_suite
        PRIVATE obfuscator_core
        PRIVATE GTest::GTest GTest::Main
    )
    add_test(NAME FullTestSuite COMMAND full_test_suite)
else()
    message(STATUS "Google Test not found, only basic tests will be built")
endif()
