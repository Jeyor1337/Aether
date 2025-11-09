#ifndef CODE_PARSER_H
#define CODE_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace obfuscator {

// 代码元素类型
enum class CodeElementType {
    FUNCTION,
    VARIABLE,
    STATEMENT,
    BLOCK,
    LOOP,
    CONDITIONAL,
    RETURN,
    CALL,
    UNKNOWN
};

// 代码元素
struct CodeElement {
    CodeElementType type;
    std::string name;
    std::string content;
    size_t startPos;
    size_t endPos;
    int lineNumber;
    std::vector<std::shared_ptr<CodeElement>> children;
};

// 函数信息
struct FunctionInfo {
    std::string name;
    std::string returnType;
    std::vector<std::string> parameters;
    std::string body;
    size_t startPos;
    size_t endPos;
    int complexity;  // 圈复杂度
};

// C/C++代码解析器
class CodeParser {
public:
    CodeParser();
    ~CodeParser();

    // 解析源代码
    bool parse(const std::string& sourceCode);

    // 获取所有函数
    std::vector<FunctionInfo> getFunctions() const { return m_functions; }

    // 获取特定函数
    FunctionInfo* getFunction(const std::string& name);

    // 获取所有变量
    std::vector<std::string> getVariables() const;

    // 获取所有字符串字面量
    std::vector<std::string> getStringLiterals() const;

    // 提取控制流图
    struct ControlFlowGraph {
        struct Node {
            std::string label;
            std::string code;
            std::vector<int> successors;
        };
        std::vector<Node> nodes;
        int entryNode = 0;
    };
    ControlFlowGraph extractCFG(const std::string& functionName);

    // 获取基本块
    std::vector<std::string> getBasicBlocks(const std::string& functionName);

    // 计算圈复杂度
    int calculateCyclomaticComplexity(const std::string& functionName);

private:
    std::string m_sourceCode;
    std::vector<FunctionInfo> m_functions;
    std::map<std::string, std::vector<std::string>> m_variables;
    std::vector<std::string> m_stringLiterals;
    std::shared_ptr<CodeElement> m_ast;

    void parseFunctions();
    void parseVariables();
    void parseStringLiterals();
    bool skipWhitespace(size_t& pos);
    bool matchKeyword(const std::string& keyword, size_t pos);
    std::string extractFunctionBody(size_t startPos);
};

// 汇编代码解析器
class AssemblyParser {
public:
    AssemblyParser();

    // 解析汇编代码
    bool parse(const std::string& asmCode);

    // 指令信息
    struct Instruction {
        std::string mnemonic;
        std::vector<std::string> operands;
        size_t address;
        std::string rawBytes;
    };

    // 获取所有指令
    std::vector<Instruction> getInstructions() const { return m_instructions; }

    // 查找特定指令
    std::vector<Instruction> findInstructions(const std::string& mnemonic);

    // 获取基本块
    struct BasicBlock {
        std::string label;
        std::vector<Instruction> instructions;
        std::vector<std::string> successors;
    };
    std::vector<BasicBlock> getBasicBlocks();

    // 查找跳转目标
    std::vector<size_t> findJumpTargets();

    // 查找函数边界
    struct FunctionBoundary {
        std::string name;
        size_t startAddr;
        size_t endAddr;
    };
    std::vector<FunctionBoundary> findFunctionBoundaries();

private:
    std::string m_asmCode;
    std::vector<Instruction> m_instructions;
    std::map<std::string, size_t> m_labels;

    Instruction parseInstruction(const std::string& line);
    bool isLabel(const std::string& line);
    bool isJumpInstruction(const std::string& mnemonic);
};

// 抽象语法树构建器
class ASTBuilder {
public:
    // 从源码构建AST
    std::shared_ptr<CodeElement> build(const std::string& sourceCode);

    // 遍历AST
    using Visitor = std::function<void(const CodeElement&)>;
    void traverse(const std::shared_ptr<CodeElement>& root, Visitor visitor);

    // 修改AST节点
    bool modifyNode(std::shared_ptr<CodeElement>& node, const std::string& newContent);

    // 插入新节点
    bool insertNode(std::shared_ptr<CodeElement>& parent,
                   std::shared_ptr<CodeElement> newChild,
                   size_t position);

    // 从AST生成代码
    std::string generateCode(const std::shared_ptr<CodeElement>& root);

private:
    std::shared_ptr<CodeElement> parseElement(const std::string& code, size_t& pos);
    void traverseImpl(const std::shared_ptr<CodeElement>& node, Visitor& visitor);
};

} // namespace obfuscator

#endif // CODE_PARSER_H
