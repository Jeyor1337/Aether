#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <memory>
#include <iomanip>

// 引入混淆器头文件
#include "engine/obfuscation_engine.h"
#include "strategy/obfuscation_strategy.h"
#include "utils/logger.h"
#include "utils/random_utils.h"

using namespace obfuscator;
using namespace obfuscator::utils;

// 打印使用帮助
void printUsage(const char* programName) {
    std::cout << "Code Obfuscator - C/C++ 花指令混淆器\n\n";
    std::cout << "用法: " << programName << " [选项]\n\n";
    std::cout << "选项:\n";
    std::cout << "  -i, --input <file>      输入源文件\n";
    std::cout << "  -o, --output <file>     输出文件\n";
    std::cout << "  -c, --config <file>     配置文件 (默认: config.json)\n";
    std::cout << "  -l, --level <1-4>       混淆等级 (1=轻度, 4=极限)\n";
    std::cout << "  -v, --verbose           详细输出\n";
    std::cout << "  -h, --help              显示此帮助信息\n";
    std::cout << "  --version               显示版本信息\n\n";
    std::cout << "示例:\n";
    std::cout << "  " << programName << " -i input.c -o output.c\n";
    std::cout << "  " << programName << " -i input.c -o output.c -l 3\n";
    std::cout << "  " << programName << " -i input.c -o output.c -c custom.json\n\n";
    std::cout << "警告: 本工具仅用于合法的软件保护和教育目的！\n";
}

void printVersion() {
    std::cout << "Code Obfuscator v1.0.0\n";
    std::cout << "基于LLVM的C/C++代码混淆工具\n";
    std::cout << "Copyright (c) 2025\n";
}

void printBanner() {
    std::cout << R"(
   ____          _        ___  _      __                 _
  / ___|___   __| | ___  / _ \| |__  / _|_   _ ___  ___| |_ ___  _ __
 | |   / _ \ / _` |/ _ \| | | | '_ \| |_| | | / __|/ __| __/ _ \| '__|
 | |__| (_) | (_| |  __/| |_| | |_) |  _| |_| \__ \ (__| || (_) | |
  \____\___/ \__,_|\___| \___/|_.__/|_|  \__,_|___/\___|\__\___/|_|

    )" << std::endl;
    std::cout << "C/C++ 花指令混淆器 - 用于合法软件保护\n" << std::endl;
}

// 真实的混淆函数
std::string obfuscateCode(const std::string& code, int level, bool verbose) {
    // 创建混淆引擎
    ObfuscationEngine engine;
    engine.setObfuscationLevel(level);
    engine.setVerbose(verbose);

    // 根据混淆等级添加策略
    if (level >= 1) {
        // Level 1: 轻度混淆 - 垃圾指令
        auto junkStrategy = std::make_unique<JunkInstructionStrategy>();
        junkStrategy->setDensity(0.2f);
        junkStrategy->setMaxPerBlock(2);
        engine.addStrategy(std::move(junkStrategy));
    }

    if (level >= 2) {
        // Level 2: 中度混淆 - 添加不透明谓词
        auto opaqueStrategy = std::make_unique<OpaquePredicateStrategy>();
        engine.addStrategy(std::move(opaqueStrategy));
    }

    if (level >= 3) {
        // Level 3: 重度混淆 - 添加字符串加密
        auto stringStrategy = std::make_unique<StringEncryptionStrategy>();
        stringStrategy->setMinLength(4);
        engine.addStrategy(std::move(stringStrategy));
    }

    if (level >= 4) {
        // Level 4: 极限混淆 - 添加控制流平坦化
        auto cfgStrategy = std::make_unique<ControlFlowFlatteningStrategy>();
        engine.addStrategy(std::move(cfgStrategy));
    }

    // 执行混淆
    std::string obfuscatedCode;
    if (!engine.obfuscate(code, obfuscatedCode)) {
        LOG_ERROR("Obfuscation failed");
        return code; // 返回原始代码
    }

    // 添加头部注释
    std::stringstream result;
    result << "/* ================================================\n";
    result << " * 混淆等级: " << level << "\n";
    result << " * 警告: 此代码已被混淆，请勿手动修改\n";
    result << " * 混淆器版本: v1.0.0\n";
    result << " * ================================================ */\n\n";
    result << obfuscatedCode;

    // 打印统计信息
    if (verbose) {
        auto stats = engine.getStatistics();
        std::cout << "\n=== 混淆统计 ===\n";
        std::cout << "原始大小: " << stats.originalSize << " 字节\n";
        std::cout << "混淆后大小: " << stats.obfuscatedSize << " 字节\n";
        std::cout << "代码膨胀率: " << std::fixed << std::setprecision(2)
                  << stats.sizeIncrease << "%\n";
        std::cout << "应用策略数: " << stats.strategiesApplied << "\n";
        std::cout << "耗时: " << std::fixed << std::setprecision(3)
                  << stats.timeTaken << " 秒\n";
    }

    return result.str();
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile;
    std::string configFile = "config.json";
    int obfuscationLevel = 2;
    bool verbose = false;

    // 如果没有参数，显示帮助
    if (argc == 1) {
        printBanner();
        printUsage(argv[0]);
        return 0;
    }

    // 解析参数
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-i" || arg == "--input") {
            if (i + 1 < argc) {
                inputFile = argv[++i];
            } else {
                std::cerr << "错误: -i 需要指定文件名\n";
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "错误: -o 需要指定文件名\n";
                return 1;
            }
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "错误: -c 需要指定配置文件名\n";
                return 1;
            }
        } else if (arg == "-l" || arg == "--level") {
            if (i + 1 < argc) {
                obfuscationLevel = std::stoi(argv[++i]);
                if (obfuscationLevel < 1 || obfuscationLevel > 4) {
                    std::cerr << "错误: 混淆等级必须在 1-4 之间\n";
                    return 1;
                }
            } else {
                std::cerr << "错误: -l 需要指定等级 (1-4)\n";
                return 1;
            }
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else {
            std::cerr << "未知选项: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // 验证必需参数
    if (inputFile.empty()) {
        std::cerr << "错误: 必须指定输入文件 (-i)\n";
        return 1;
    }

    if (outputFile.empty()) {
        std::cerr << "错误: 必须指定输出文件 (-o)\n";
        return 1;
    }

    // 配置日志系统
    Logger& logger = Logger::getInstance();
    if (verbose) {
        logger.setLogLevel(LogLevel::DEBUG);
        logger.setConsoleOutput(true);
        printBanner();
    } else {
        logger.setLogLevel(LogLevel::ERROR);
        logger.setConsoleOutput(false);
    }

    // 读取输入文件
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "错误: 无法打开输入文件: " << inputFile << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string sourceCode = buffer.str();
    inFile.close();

    if (verbose) {
        std::cout << "\n=== 配置信息 ===\n";
        std::cout << "输入文件: " << inputFile << " (" << sourceCode.size() << " 字节)\n";
        std::cout << "输出文件: " << outputFile << "\n";
        std::cout << "混淆等级: " << obfuscationLevel << "\n";
        std::cout << "配置文件: " << configFile << "\n\n";
        std::cout << "开始混淆...\n\n";
    }

    // 执行混淆
    std::string obfuscatedCode = obfuscateCode(sourceCode, obfuscationLevel, verbose);

    // 写入输出文件
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "错误: 无法创建输出文件: " << outputFile << "\n";
        return 1;
    }

    outFile << obfuscatedCode;
    outFile.close();

    if (verbose) {
        std::cout << "\n=== 完成 ===\n";
        std::cout << "混淆完成！输出已保存到: " << outputFile << "\n";
    } else {
        std::cout << "成功: " << inputFile << " -> " << outputFile << "\n";
    }

    return 0;
}
