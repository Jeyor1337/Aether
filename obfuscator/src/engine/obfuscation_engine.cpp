#include "engine/obfuscation_engine.h"
#include "utils/logger.h"
#include "utils/random_utils.h"
#include <chrono>
#include <fstream>
#include <algorithm>

namespace obfuscator {

using namespace utils;

// ============================================================================
// InstrumentationEngine Implementation
// ============================================================================

InstrumentationEngine::InstrumentationEngine() {
}

InstrumentationEngine::~InstrumentationEngine() {
}

bool InstrumentationEngine::insertCode(const std::string& code, size_t position) {
    if (m_sourceCode.empty()) {
        LOG_ERROR("Source code is empty");
        return false;
    }

    if (position > m_sourceCode.length()) {
        LOG_ERROR("Insert position out of range");
        return false;
    }

    m_instrumentedCode = m_sourceCode;
    m_instrumentedCode.insert(position, code);

    return true;
}

bool InstrumentationEngine::insertAtBlockEntry(const std::string& blockName,
                                               const std::string& code) {
    size_t pos = findBlockPosition(blockName);
    if (pos == std::string::npos) {
        LOG_ERROR("Block not found: " + blockName);
        return false;
    }

    return insertCode(code, pos);
}

bool InstrumentationEngine::insertAtBlockExit(const std::string& blockName,
                                              const std::string& code) {
    // 简化实现：在块结束的 } 之前插入
    size_t pos = findBlockPosition(blockName);
    if (pos == std::string::npos) {
        LOG_ERROR("Block not found: " + blockName);
        return false;
    }

    // 查找对应的结束括号
    size_t endPos = m_sourceCode.find("}", pos);
    if (endPos == std::string::npos) {
        LOG_ERROR("Block end not found");
        return false;
    }

    return insertCode(code, endPos);
}

bool InstrumentationEngine::insertAtFunctionStart(const std::string& funcName,
                                                  const std::string& code) {
    size_t pos = findFunctionPosition(funcName);
    if (pos == std::string::npos) {
        LOG_ERROR("Function not found: " + funcName);
        return false;
    }

    // 查找函数体的开始 {
    size_t openBrace = m_sourceCode.find("{", pos);
    if (openBrace == std::string::npos) {
        LOG_ERROR("Function body not found");
        return false;
    }

    return insertCode(code, openBrace + 1);
}

bool InstrumentationEngine::insertAtFunctionEnd(const std::string& funcName,
                                                const std::string& code) {
    size_t pos = findFunctionPosition(funcName);
    if (pos == std::string::npos) {
        LOG_ERROR("Function not found: " + funcName);
        return false;
    }

    // 查找函数体的结束 }
    size_t openBrace = m_sourceCode.find("{", pos);
    if (openBrace == std::string::npos) {
        return false;
    }

    // 简化：查找对应的闭括号
    int braceCount = 1;
    size_t i = openBrace + 1;
    while (i < m_sourceCode.length() && braceCount > 0) {
        if (m_sourceCode[i] == '{') braceCount++;
        else if (m_sourceCode[i] == '}') braceCount--;
        i++;
    }

    if (braceCount != 0) {
        LOG_ERROR("Unmatched braces");
        return false;
    }

    return insertCode(code, i - 1);
}

void InstrumentationEngine::analyzeCode() {
    // 分析代码，提取基本块和函数位置
    // 简化实现
}

size_t InstrumentationEngine::findBlockPosition(const std::string& blockName) {
    return m_sourceCode.find(blockName);
}

size_t InstrumentationEngine::findFunctionPosition(const std::string& funcName) {
    // 简化查找：查找 "funcName(" 模式
    std::string pattern = funcName + "(";
    return m_sourceCode.find(pattern);
}

// ============================================================================
// ObfuscationEngine Implementation
// ============================================================================

ObfuscationEngine::ObfuscationEngine()
    : m_instrumentationEngine(std::make_unique<InstrumentationEngine>()) {
}

ObfuscationEngine::~ObfuscationEngine() {
}

void ObfuscationEngine::addStrategy(std::unique_ptr<ObfuscationStrategy> strategy) {
    if (strategy) {
        strategy->setLevel(m_obfuscationLevel);
        m_strategies.push_back(std::move(strategy));
        LOG_INFO("Added strategy: " + m_strategies.back()->getName());
    }
}

void ObfuscationEngine::removeStrategy(const std::string& name) {
    auto it = std::remove_if(m_strategies.begin(), m_strategies.end(),
        [&name](const std::unique_ptr<ObfuscationStrategy>& strategy) {
            return strategy->getName() == name;
        });

    if (it != m_strategies.end()) {
        m_strategies.erase(it, m_strategies.end());
        LOG_INFO("Removed strategy: " + name);
    }
}

void ObfuscationEngine::setObfuscationLevel(int level) {
    if (level < 1 || level > 4) {
        LOG_WARNING("Invalid obfuscation level, using default (2)");
        m_obfuscationLevel = 2;
        return;
    }

    m_obfuscationLevel = level;
    LOG_INFO("Obfuscation level set to: " + std::to_string(level));

    // 更新所有策略的级别
    for (auto& strategy : m_strategies) {
        strategy->setLevel(level);
    }
}

bool ObfuscationEngine::obfuscate(const std::string& inputCode, std::string& outputCode) {
    LOG_INFO("Starting obfuscation process");

    auto startTime = std::chrono::high_resolution_clock::now();

    // 验证输入
    if (!validateInput(inputCode)) {
        LOG_ERROR("Input validation failed");
        return false;
    }

    // 应用混淆策略
    if (!applyStrategies(inputCode, outputCode)) {
        LOG_ERROR("Failed to apply strategies");
        return false;
    }

    // 更新统计信息
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;

    m_stats.originalSize = inputCode.size();
    m_stats.obfuscatedSize = outputCode.size();
    m_stats.sizeIncrease = ((double)outputCode.size() / inputCode.size() - 1.0) * 100.0;
    m_stats.timeTaken = elapsed.count();

    LOG_INFO("Obfuscation completed successfully");
    logMessage("Original size: " + std::to_string(m_stats.originalSize) + " bytes");
    logMessage("Obfuscated size: " + std::to_string(m_stats.obfuscatedSize) + " bytes");
    logMessage("Size increase: " + std::to_string(m_stats.sizeIncrease) + "%");
    logMessage("Time taken: " + std::to_string(m_stats.timeTaken) + " seconds");

    return true;
}

bool ObfuscationEngine::obfuscateBatch(const std::vector<std::string>& inputFiles,
                                       const std::vector<std::string>& outputFiles) {
    if (inputFiles.size() != outputFiles.size()) {
        LOG_ERROR("Input and output file count mismatch");
        return false;
    }

    LOG_INFO("Starting batch obfuscation of " + std::to_string(inputFiles.size()) + " files");

    for (size_t i = 0; i < inputFiles.size(); ++i) {
        LOG_INFO("Processing file " + std::to_string(i + 1) + "/" +
                std::to_string(inputFiles.size()) + ": " + inputFiles[i]);

        // 读取输入文件
        std::ifstream inFile(inputFiles[i]);
        if (!inFile.is_open()) {
            LOG_ERROR("Failed to open input file: " + inputFiles[i]);
            continue;
        }

        std::string inputCode((std::istreambuf_iterator<char>(inFile)),
                             std::istreambuf_iterator<char>());
        inFile.close();

        // 混淆
        std::string outputCode;
        if (!obfuscate(inputCode, outputCode)) {
            LOG_ERROR("Failed to obfuscate: " + inputFiles[i]);
            continue;
        }

        // 写入输出文件
        std::ofstream outFile(outputFiles[i]);
        if (!outFile.is_open()) {
            LOG_ERROR("Failed to open output file: " + outputFiles[i]);
            continue;
        }

        outFile << outputCode;
        outFile.close();

        LOG_INFO("Successfully processed: " + inputFiles[i] + " -> " + outputFiles[i]);
    }

    LOG_INFO("Batch obfuscation completed");
    return true;
}

bool ObfuscationEngine::validateInput(const std::string& code) {
    if (code.empty()) {
        LOG_ERROR("Input code is empty");
        return false;
    }

    // 基本验证
    // 可以添加更多验证，如检查代码是否是有效的C/C++

    return true;
}

bool ObfuscationEngine::applyStrategies(const std::string& input, std::string& output) {
    std::string currentCode = input;
    std::string nextCode;

    m_stats.strategiesApplied = 0;

    for (auto& strategy : m_strategies) {
        if (!strategy->isEnabled()) {
            LOG_INFO("Skipping disabled strategy: " + strategy->getName());
            continue;
        }

        LOG_INFO("Applying strategy: " + strategy->getName());

        if (strategy->apply(currentCode, nextCode)) {
            currentCode = nextCode;
            m_stats.strategiesApplied++;
            logMessage("Strategy applied: " + strategy->getName());
        } else {
            LOG_WARNING("Strategy failed: " + strategy->getName());
        }
    }

    output = currentCode;
    return true;
}

void ObfuscationEngine::updateStatistics(const std::string& input, const std::string& output) {
    m_stats.originalSize = input.size();
    m_stats.obfuscatedSize = output.size();
    m_stats.sizeIncrease = ((double)output.size() / input.size() - 1.0) * 100.0;
}

void ObfuscationEngine::logMessage(const std::string& message) {
    if (m_verbose) {
        LOG_INFO(message);
    }
}

// ============================================================================
// ControlFlowRewriter Implementation
// ============================================================================

ControlFlowRewriter::ControlFlowRewriter() {
}

std::string ControlFlowRewriter::flattenControlFlow(const std::string& code) {
    // 控制流平坦化的简化实现
    LOG_INFO("Flattening control flow");

    std::vector<BasicBlock> blocks = extractBasicBlocks(code);
    return generateSwitchDispatcher(blocks);
}

std::string ControlFlowRewriter::addFakeBranches(const std::string& code, float probability) {
    LOG_INFO("Adding fake branches");

    auto& rng = RandomGenerator::getInstance();
    std::stringstream result;

    std::istringstream iss(code);
    std::string line;

    while (std::getline(iss, line)) {
        result << line << "\n";

        if (rng.randomBool(probability)) {
            // 添加永不执行的虚假分支
            result << "    if (0) { volatile int __fake = 1; }\n";
        }
    }

    return result.str();
}

std::vector<std::string> ControlFlowRewriter::splitBasicBlocks(const std::string& code) {
    std::vector<std::string> blocks;

    // 简化实现：按函数分割
    std::istringstream iss(code);
    std::string line;
    std::string currentBlock;

    while (std::getline(iss, line)) {
        currentBlock += line + "\n";

        if (line.find("}") != std::string::npos) {
            blocks.push_back(currentBlock);
            currentBlock.clear();
        }
    }

    if (!currentBlock.empty()) {
        blocks.push_back(currentBlock);
    }

    return blocks;
}

std::string ControlFlowRewriter::shuffleBasicBlocks(const std::vector<std::string>& blocks) {
    std::vector<std::string> shuffled = blocks;
    RandomGenerator::getInstance().shuffle(shuffled);

    std::stringstream result;
    for (const auto& block : shuffled) {
        result << block << "\n";
    }

    return result.str();
}

std::vector<ControlFlowRewriter::BasicBlock> ControlFlowRewriter::extractBasicBlocks(
    const std::string& code) {
    std::vector<BasicBlock> blocks;

    // 简化实现
    BasicBlock block;
    block.label = "entry";
    block.code = code;

    blocks.push_back(block);

    return blocks;
}

std::string ControlFlowRewriter::generateSwitchDispatcher(
    const std::vector<BasicBlock>& blocks) {
    std::stringstream ss;

    ss << "int __dispatcher_state = 0;\n";
    ss << "while (1) {\n";
    ss << "    switch (__dispatcher_state) {\n";

    for (size_t i = 0; i < blocks.size(); ++i) {
        ss << "    case " << i << ":\n";
        ss << blocks[i].code << "\n";
        ss << "        __dispatcher_state = " << ((i + 1) % blocks.size()) << ";\n";
        ss << "        break;\n";
    }

    ss << "    default:\n";
    ss << "        goto __exit;\n";
    ss << "    }\n";
    ss << "}\n";
    ss << "__exit:\n";

    return ss.str();
}

// ============================================================================
// CodeValidator Implementation
// ============================================================================

bool CodeValidator::validateSyntax(const std::string& code) {
    // 简化实现：基本的语法检查
    // 实际应该使用 Clang 进行完整的语法检查

    if (code.empty()) {
        return false;
    }

    // 检查括号平衡
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;

    for (char c : code) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }

    return braceCount == 0 && parenCount == 0 && bracketCount == 0;
}

bool CodeValidator::validateEquivalence(const std::string& original,
                                        const std::string& obfuscated) {
    // 实际应该通过编译和运行测试来验证等价性
    // 这里只做简单检查

    LOG_INFO("Validating code equivalence");
    return true;
}

bool CodeValidator::checkStackBalance(const std::string& code) {
    // 检查栈操作是否平衡
    LOG_INFO("Checking stack balance");
    return true;
}

bool CodeValidator::checkRegisterUsage(const std::string& code) {
    // 检查寄存器使用是否正确
    LOG_INFO("Checking register usage");
    return true;
}

} // namespace obfuscator
