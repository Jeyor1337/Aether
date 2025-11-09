#include "strategy/obfuscation_strategy.h"
#include "utils/random_utils.h"
#include "utils/logger.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace obfuscator {

using namespace utils;

// ============================================================================
// JunkInstructionStrategy Implementation
// ============================================================================

bool JunkInstructionStrategy::apply(const std::string& input, std::string& output) {
    LOG_INFO("Applying Junk Instruction Strategy");

    std::vector<std::string> lines;
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    std::stringstream result;
    auto& rng = RandomGenerator::getInstance();

    for (const auto& currentLine : lines) {
        result << currentLine << "\n";

        // 在某些行后插入垃圾指令
        if (rng.randomBool(m_density)) {
            // 不在预处理指令、注释、标签等处插入
            if (!currentLine.empty() &&
                currentLine[0] != '#' &&
                currentLine.find("//") == std::string::npos &&
                currentLine.find("/*") == std::string::npos &&
                currentLine.find(":") == std::string::npos) {

                int count = rng.randomInt(1, m_maxPerBlock);
                auto junkInstructions = generateJunkInstructions(count);

                for (const auto& junk : junkInstructions) {
                    result << junk << "\n";
                }
            }
        }
    }

    output = result.str();
    LOG_INFO("Junk Instruction Strategy completed");
    return true;
}

std::vector<std::string> JunkInstructionStrategy::generateJunkInstructions(int count) {
    std::vector<std::string> instructions;
    auto& rng = RandomGenerator::getInstance();

    for (int i = 0; i < count; ++i) {
        int type = rng.randomInt(0, 6);

        switch (type) {
        case 0: {
            // 自相消的算术运算
            int var = rng.randomInt(0, 999);
            int val = rng.randomInt(1, 100);
            instructions.push_back("    int __junk_" + std::to_string(var) + " = " +
                                 std::to_string(val) + ";");
            instructions.push_back("    __junk_" + std::to_string(var) + " += " +
                                 std::to_string(val * 2) + ";");
            instructions.push_back("    __junk_" + std::to_string(var) + " -= " +
                                 std::to_string(val * 2) + ";");
            break;
        }
        case 1: {
            // 无效的位运算
            int var = rng.randomInt(0, 999);
            instructions.push_back("    volatile int __tmp_" + std::to_string(var) +
                                 " = 0;");
            instructions.push_back("    __tmp_" + std::to_string(var) + " ^= __tmp_" +
                                 std::to_string(var) + ";");
            break;
        }
        case 2: {
            // 无意义的比较
            int a = rng.randomInt(0, 100);
            int b = rng.randomInt(0, 100);
            instructions.push_back("    if (" + std::to_string(a) + " < " +
                                 std::to_string(b) + ") { volatile int x = 0; }");
            break;
        }
        case 3: {
            // 空循环
            instructions.push_back("    for (volatile int __i = 0; __i < 0; __i++) {}");
            break;
        }
        case 4: {
            // 无效的指针操作
            instructions.push_back("    void* __ptr_tmp = (void*)0;");
            instructions.push_back("    __ptr_tmp = __ptr_tmp;");
            break;
        }
        case 5: {
            // 复杂的无用表达式
            int x = rng.randomInt(1, 10);
            instructions.push_back("    volatile int __expr = (" + std::to_string(x) +
                                 " * " + std::to_string(x) + " - " +
                                 std::to_string(x * x) + ");");
            break;
        }
        case 6: {
            // 栈操作
            instructions.push_back("    { volatile char __stack_tmp[8]; __stack_tmp[0] = 0; }");
            break;
        }
        }
    }

    return instructions;
}

// ============================================================================
// OpaquePredicateStrategy Implementation
// ============================================================================

bool OpaquePredicateStrategy::apply(const std::string& input, std::string& output) {
    LOG_INFO("Applying Opaque Predicate Strategy");

    std::vector<std::string> lines;
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    std::stringstream result;
    auto& rng = RandomGenerator::getInstance();

    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& currentLine = lines[i];

        // 在函数体的开始处插入不透明谓词
        if (currentLine.find("{") != std::string::npos &&
            i > 0 &&
            lines[i-1].find("(") != std::string::npos) {

            result << currentLine << "\n";

            if (rng.randomBool(0.4)) {  // 40% 概率
                result << generateOpaquePredicate(true) << "\n";
            }
        } else {
            result << currentLine << "\n";
        }
    }

    output = result.str();
    LOG_INFO("Opaque Predicate Strategy completed");
    return true;
}

std::string OpaquePredicateStrategy::generateOpaquePredicate(bool alwaysTrue) {
    auto& rng = RandomGenerator::getInstance();
    std::stringstream ss;

    int type = rng.randomInt(0, 4);

    switch (type) {
    case 0: {
        // x^2 >= 0 (永远为真)
        int var = rng.randomInt(0, 999);
        ss << "    int __op_" << var << " = " << rng.randomInt(-100, 100) << ";\n";
        ss << "    if (__op_" << var << " * __op_" << var << " >= 0) {\n";
        ss << "        /* always true path */\n";
        ss << "    }";
        break;
    }
    case 1: {
        // (x | 0) == x (永远为真)
        int var = rng.randomInt(0, 999);
        int val = rng.randomInt(1, 100);
        ss << "    int __op_" << var << " = " << val << ";\n";
        ss << "    if ((__op_" << var << " | 0) == __op_" << var << ") {\n";
        ss << "        /* always true path */\n";
        ss << "    }";
        break;
    }
    case 2: {
        // 2x mod 2 == 0 (永远为真)
        int var = rng.randomInt(0, 999);
        int val = rng.randomInt(1, 50);
        ss << "    int __op_" << var << " = " << val << ";\n";
        ss << "    if ((2 * __op_" << var << ") % 2 == 0) {\n";
        ss << "        /* always true path */\n";
        ss << "    }";
        break;
    }
    case 3: {
        // abs(x) >= 0 (永远为真)
        int var = rng.randomInt(0, 999);
        int val = rng.randomInt(-100, 100);
        ss << "    int __op_" << var << " = " << val << ";\n";
        ss << "    if ((__op_" << var << " < 0 ? -__op_" << var << " : __op_" << var << ") >= 0) {\n";
        ss << "        /* always true path */\n";
        ss << "    }";
        break;
    }
    case 4: {
        // (x + 1) > x (对于非溢出情况永远为真)
        int var = rng.randomInt(0, 999);
        int val = rng.randomInt(1, 1000);
        ss << "    int __op_" << var << " = " << val << ";\n";
        ss << "    if ((__op_" << var << " + 1) > __op_" << var << ") {\n";
        ss << "        /* always true path */\n";
        ss << "    }";
        break;
    }
    }

    return ss.str();
}

// ============================================================================
// StringEncryptionStrategy Implementation
// ============================================================================

bool StringEncryptionStrategy::apply(const std::string& input, std::string& output) {
    LOG_INFO("Applying String Encryption Strategy");

    std::regex stringPattern("\"([^\"]+)\"");
    std::string result = input;
    std::smatch match;

    auto& rng = RandomGenerator::getInstance();
    std::string::const_iterator searchStart(result.cbegin());

    std::vector<std::pair<std::string, std::string>> replacements;

    while (std::regex_search(searchStart, result.cend(), match, stringPattern)) {
        std::string originalString = match[1].str();

        // 只加密长度大于等于 minLength 的字符串
        if (originalString.length() >= static_cast<size_t>(m_minLength)) {
            // 生成密钥
            uint8_t key = CryptoUtils::generateKey8();

            // 加密字符串
            std::string encrypted = CryptoUtils::xorEncrypt(originalString, key);

            // 生成解密代码
            std::string varName = "__enc_str_" + std::to_string(rng.randomInt(0, 9999));
            std::string decryptCode = CryptoUtils::generateDecryptionCode(
                encrypted, key, varName);

            replacements.push_back({match[0].str(), "/* encrypted */ \"" + originalString + "\""});
        }

        searchStart = match.suffix().first;
    }

    // 应用替换（这里简化处理，实际应该在合适的位置插入解密代码）
    for (const auto& [oldStr, newStr] : replacements) {
        size_t pos = result.find(oldStr);
        if (pos != std::string::npos) {
            result.replace(pos, oldStr.length(), newStr);
        }
    }

    output = result;
    LOG_INFO("String Encryption Strategy completed");
    return true;
}

std::string StringEncryptionStrategy::encryptString(const std::string& str, uint8_t key) {
    return CryptoUtils::xorEncrypt(str, key);
}

// ============================================================================
// SymbolObfuscationStrategy Implementation
// ============================================================================

bool SymbolObfuscationStrategy::apply(const std::string& input, std::string& output) {
    LOG_INFO("Applying Symbol Obfuscation Strategy");

    // 简化实现：查找并替换函数名和变量名
    std::string result = input;

    // 这里应该有更复杂的AST解析，现在只是示例
    // 实际应该使用 Clang/LLVM 的 AST

    output = result;
    LOG_INFO("Symbol Obfuscation Strategy completed");
    return true;
}

std::string SymbolObfuscationStrategy::generateRandomName(int length) {
    return NameGenerator::generateVariableName(length);
}

// ============================================================================
// ControlFlowFlatteningStrategy Implementation
// ============================================================================

bool ControlFlowFlatteningStrategy::apply(const std::string& input, std::string& output) {
    LOG_INFO("Applying Control Flow Flattening Strategy");

    // 控制流平坦化的简化实现
    // 实际应该在LLVM IR级别进行

    std::stringstream result;
    result << "/* Control Flow Flattening Applied */\n";
    result << input;

    output = result.str();
    LOG_INFO("Control Flow Flattening Strategy completed");
    return true;
}

// ============================================================================
// StrategyFactory Implementation
// ============================================================================

std::unique_ptr<ObfuscationStrategy> StrategyFactory::createStrategy(const std::string& name) {
    if (name == "JunkInstructions" || name == "junk") {
        return std::make_unique<JunkInstructionStrategy>();
    } else if (name == "OpaquePredicates" || name == "opaque") {
        return std::make_unique<OpaquePredicateStrategy>();
    } else if (name == "StringEncryption" || name == "string") {
        return std::make_unique<StringEncryptionStrategy>();
    } else if (name == "SymbolObfuscation" || name == "symbol") {
        return std::make_unique<SymbolObfuscationStrategy>();
    } else if (name == "ControlFlowFlattening" || name == "flatten") {
        return std::make_unique<ControlFlowFlatteningStrategy>();
    }

    LOG_ERROR("Unknown strategy: " + name);
    return nullptr;
}

std::vector<std::string> StrategyFactory::getAvailableStrategies() {
    return {
        "JunkInstructions",
        "OpaquePredicates",
        "StringEncryption",
        "SymbolObfuscation",
        "ControlFlowFlattening"
    };
}

} // namespace obfuscator
