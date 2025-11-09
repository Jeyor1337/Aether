cmake_minimum_required(VERSION 3.15)
project(CodeObfuscator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 项目选项
option(BUILD_LLVM_PASS "Build LLVM Pass obfuscation module" ON)
option(BUILD_ASM_REWRITER "Build Assembly rewriter" ON)
option(BUILD_TESTS "Build test suite" ON)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# 编译器警告设置
if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

# 查找依赖
find_package(LLVM QUIET)
if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
else()
    message(WARNING "LLVM not found, LLVM Pass will not be built")
    set(BUILD_LLVM_PASS OFF)
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 核心库源文件
set(CORE_SOURCES
    src/config/config_manager.cpp
    src/asm_rewriter/asm_rewriter.cpp
    src/utils/random_utils.cpp
    src/utils/logger.cpp
    src/strategy/obfuscation_strategy.cpp
    src/engine/obfuscation_engine.cpp
    src/parser/code_parser.cpp
)

# 创建核心库
add_library(obfuscator_core STATIC ${CORE_SOURCES})
target_include_directories(obfuscator_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 链接线程库(logger需要)
find_package(Threads REQUIRED)
target_link_libraries(obfuscator_core PUBLIC Threads::Threads)

# LLVM Pass 模块
if(BUILD_LLVM_PASS)
    add_subdirectory(src/llvm_pass)
endif()

# CLI 可执行文件
add_executable(obfuscator-cli
    src/main.cpp
)
target_link_libraries(obfuscator-cli PRIVATE obfuscator_core)

# 安装规则
install(TARGETS obfuscator-cli DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/obfuscator)
install(DIRECTORY templates/ DESTINATION share/obfuscator/templates)

# 测试
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 打印配置信息
message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  Build LLVM Pass: ${BUILD_LLVM_PASS}")
message(STATUS "  Build Assembly Rewriter: ${BUILD_ASM_REWRITER}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "")
