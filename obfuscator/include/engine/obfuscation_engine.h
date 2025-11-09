#ifndef OBFUSCATION_ENGINE_H
#define OBFUSCATION_ENGINE_H

#include "strategy/obfuscation_strategy.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace obfuscator {

// 代码插桩引擎
class InstrumentationEngine {
public:
    InstrumentationEngine();
    ~InstrumentationEngine();

    // 在指定位置插入代码
    bool insertCode(const std::string& code, size_t position);

    // 在基本块入口插入
    bool insertAtBlockEntry(const std::string& blockName, const std::string& code);

    // 在基本块出口插入
    bool insertAtBlockExit(const std::string& blockName, const std::string& code);

    // 在函数开头插入
    bool insertAtFunctionStart(const std::string& funcName, const std::string& code);

    // 在函数结尾插入
    bool insertAtFunctionEnd(const std::string& funcName, const std::string& code);

    // 获取插桩后的代码
    std::string getInstrumentedCode() const { return m_instrumentedCode; }

    // 设置源代码
    void setSourceCode(const std::string& code) { m_sourceCode = code; }

private:
    std::string m_sourceCode;
    std::string m_instrumentedCode;
    std::map<std::string, size_t> m_blockPositions;
    std::map<std::string, size_t> m_functionPositions;

    void analyzeCode();
    size_t findBlockPosition(const std::string& blockName);
    size_t findFunctionPosition(const std::string& funcName);
};

// 混淆引擎核心
class ObfuscationEngine {
public:
    ObfuscationEngine();
    ~ObfuscationEngine();

    // 添加混淆策略
    void addStrategy(std::unique_ptr<ObfuscationStrategy> strategy);

    // 移除策略
    void removeStrategy(const std::string& name);

    // 设置混淆等级 (1-4)
    void setObfuscationLevel(int level);

    // 执行混淆
    bool obfuscate(const std::string& inputCode, std::string& outputCode);

    // 批量处理
    bool obfuscateBatch(const std::vector<std::string>& inputFiles,
                       const std::vector<std::string>& outputFiles);

    // 设置是否保留调试信息
    void setPreserveDebugInfo(bool preserve) { m_preserveDebugInfo = preserve; }

    // 设置详细输出
    void setVerbose(bool verbose) { m_verbose = verbose; }

    // 获取统计信息
    struct Statistics {
        size_t originalSize = 0;
        size_t obfuscatedSize = 0;
        double sizeIncrease = 0.0;
        int strategiesApplied = 0;
        double timeTaken = 0.0;
    };
    Statistics getStatistics() const { return m_stats; }

private:
    std::vector<std::unique_ptr<ObfuscationStrategy>> m_strategies;
    std::unique_ptr<InstrumentationEngine> m_instrumentationEngine;
    int m_obfuscationLevel = 2;
    bool m_preserveDebugInfo = false;
    bool m_verbose = false;
    Statistics m_stats;

    bool validateInput(const std::string& code);
    bool applyStrategies(const std::string& input, std::string& output);
    void updateStatistics(const std::string& input, const std::string& output);
    void logMessage(const std::string& message);
};

// 控制流重构器
class ControlFlowRewriter {
public:
    ControlFlowRewriter();

    // 平坦化控制流
    std::string flattenControlFlow(const std::string& code);

    // 添加虚假分支
    std::string addFakeBranches(const std::string& code, float probability = 0.2f);

    // 分割基本块
    std::vector<std::string> splitBasicBlocks(const std::string& code);

    // 重组基本块（随机化顺序）
    std::string shuffleBasicBlocks(const std::vector<std::string>& blocks);

private:
    struct BasicBlock {
        std::string label;
        std::string code;
        std::vector<std::string> successors;
    };

    std::vector<BasicBlock> extractBasicBlocks(const std::string& code);
    std::string generateSwitchDispatcher(const std::vector<BasicBlock>& blocks);
};

// 代码验证器
class CodeValidator {
public:
    // 验证代码语法正确性
    static bool validateSyntax(const std::string& code);

    // 验证混淆前后功能等价性
    static bool validateEquivalence(const std::string& original,
                                   const std::string& obfuscated);

    // 检查栈平衡
    static bool checkStackBalance(const std::string& code);

    // 检查寄存器使用
    static bool checkRegisterUsage(const std::string& code);
};

} // namespace obfuscator

#endif // OBFUSCATION_ENGINE_H
