#include "parser/code_parser.h"
#include "utils/logger.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <functional>

namespace obfuscator {

using namespace utils;

// ============================================================================
// CodeParser Implementation
// ============================================================================

CodeParser::CodeParser() {
}

CodeParser::~CodeParser() {
}

bool CodeParser::parse(const std::string& sourceCode) {
    if (sourceCode.empty()) {
        LOG_ERROR("Source code is empty");
        return false;
    }

    m_sourceCode = sourceCode;

    // 清除之前的解析结果
    m_functions.clear();
    m_variables.clear();
    m_stringLiterals.clear();

    // 解析各种元素
    parseFunctions();
    parseVariables();
    parseStringLiterals();

    LOG_INFO("Code parsing completed");
    return true;
}

FunctionInfo* CodeParser::getFunction(const std::string& name) {
    for (auto& func : m_functions) {
        if (func.name == name) {
            return &func;
        }
    }
    return nullptr;
}

std::vector<std::string> CodeParser::getVariables() const {
    std::vector<std::string> allVars;

    for (const auto& [funcName, vars] : m_variables) {
        allVars.insert(allVars.end(), vars.begin(), vars.end());
    }

    return allVars;
}

std::vector<std::string> CodeParser::getStringLiterals() const {
    return m_stringLiterals;
}

CodeParser::ControlFlowGraph CodeParser::extractCFG(const std::string& functionName) {
    ControlFlowGraph cfg;

    // 简化实现：创建基本的CFG
    FunctionInfo* func = getFunction(functionName);
    if (!func) {
        LOG_ERROR("Function not found: " + functionName);
        return cfg;
    }

    // 创建单个节点表示整个函数
    ControlFlowGraph::Node node;
    node.label = "entry";
    node.code = func->body;

    cfg.nodes.push_back(node);
    cfg.entryNode = 0;

    return cfg;
}

std::vector<std::string> CodeParser::getBasicBlocks(const std::string& functionName) {
    std::vector<std::string> blocks;

    FunctionInfo* func = getFunction(functionName);
    if (!func) {
        LOG_ERROR("Function not found: " + functionName);
        return blocks;
    }

    // 简化实现：整个函数体作为一个基本块
    blocks.push_back(func->body);

    return blocks;
}

int CodeParser::calculateCyclomaticComplexity(const std::string& functionName) {
    FunctionInfo* func = getFunction(functionName);
    if (!func) {
        LOG_ERROR("Function not found: " + functionName);
        return 0;
    }

    // 简化实现：计算决策点数量
    int complexity = 1; // 基础复杂度

    // 计算 if, while, for, switch, case 等关键字出现次数
    std::vector<std::string> keywords = {"if", "while", "for", "case", "&&", "||"};

    for (const auto& keyword : keywords) {
        size_t pos = 0;
        while ((pos = func->body.find(keyword, pos)) != std::string::npos) {
            complexity++;
            pos += keyword.length();
        }
    }

    func->complexity = complexity;
    return complexity;
}

void CodeParser::parseFunctions() {
    // 简化的函数解析：使用正则表达式
    // 格式: returnType functionName(parameters) { body }

    std::regex funcPattern(
        R"((\w+)\s+(\w+)\s*\(([^)]*)\)\s*\{)"
    );

    std::smatch match;
    std::string::const_iterator searchStart(m_sourceCode.cbegin());

    while (std::regex_search(searchStart, m_sourceCode.cend(), match, funcPattern)) {
        FunctionInfo func;
        func.returnType = match[1].str();
        func.name = match[2].str();

        // 解析参数
        std::string params = match[3].str();
        if (!params.empty()) {
            std::stringstream ss(params);
            std::string param;
            while (std::getline(ss, param, ',')) {
                // 移除前后空白
                param.erase(0, param.find_first_not_of(" \t"));
                param.erase(param.find_last_not_of(" \t") + 1);
                if (!param.empty()) {
                    func.parameters.push_back(param);
                }
            }
        }

        // 获取函数体的起始位置
        func.startPos = match.position() + match.length();

        // 提取函数体
        func.body = extractFunctionBody(func.startPos);

        func.endPos = func.startPos + func.body.length();

        m_functions.push_back(func);

        searchStart = match.suffix().first;
    }

    LOG_INFO("Found " + std::to_string(m_functions.size()) + " functions");
}

void CodeParser::parseVariables() {
    // 简化的变量解析
    // 查找类型声明：int x, char* str, 等

    std::regex varPattern(R"((int|char|float|double|void\*|long)\s+(\w+))");

    std::smatch match;
    std::string::const_iterator searchStart(m_sourceCode.cbegin());

    while (std::regex_search(searchStart, m_sourceCode.cend(), match, varPattern)) {
        std::string varName = match[2].str();

        // 简化：将变量添加到全局列表
        m_variables["global"].push_back(varName);

        searchStart = match.suffix().first;
    }

    LOG_INFO("Found " + std::to_string(m_variables["global"].size()) + " variables");
}

void CodeParser::parseStringLiterals() {
    // 查找所有字符串字面量

    std::regex stringPattern("\"([^\"]*)\"");

    std::smatch match;
    std::string::const_iterator searchStart(m_sourceCode.cbegin());

    while (std::regex_search(searchStart, m_sourceCode.cend(), match, stringPattern)) {
        m_stringLiterals.push_back(match[1].str());
        searchStart = match.suffix().first;
    }

    LOG_INFO("Found " + std::to_string(m_stringLiterals.size()) + " string literals");
}

bool CodeParser::skipWhitespace(size_t& pos) {
    while (pos < m_sourceCode.length() &&
           std::isspace(m_sourceCode[pos])) {
        pos++;
    }
    return pos < m_sourceCode.length();
}

bool CodeParser::matchKeyword(const std::string& keyword, size_t pos) {
    if (pos + keyword.length() > m_sourceCode.length()) {
        return false;
    }

    return m_sourceCode.substr(pos, keyword.length()) == keyword;
}

std::string CodeParser::extractFunctionBody(size_t startPos) {
    // 提取从 { 到匹配的 } 之间的内容

    int braceCount = 1;
    size_t pos = startPos;

    while (pos < m_sourceCode.length() && braceCount > 0) {
        if (m_sourceCode[pos] == '{') {
            braceCount++;
        } else if (m_sourceCode[pos] == '}') {
            braceCount--;
        }
        pos++;
    }

    if (braceCount != 0) {
        LOG_ERROR("Unmatched braces in function body");
        return "";
    }

    return m_sourceCode.substr(startPos, pos - startPos - 1);
}

// ============================================================================
// AssemblyParser Implementation
// ============================================================================

AssemblyParser::AssemblyParser() {
}

bool AssemblyParser::parse(const std::string& asmCode) {
    if (asmCode.empty()) {
        LOG_ERROR("Assembly code is empty");
        return false;
    }

    m_asmCode = asmCode;
    m_instructions.clear();
    m_labels.clear();

    // 逐行解析
    std::istringstream iss(asmCode);
    std::string line;
    size_t address = 0;

    while (std::getline(iss, line)) {
        // 移除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == ';') {
            // 跳过空行和注释
            continue;
        }

        if (isLabel(line)) {
            // 记录标签位置
            std::string label = line.substr(0, line.find(':'));
            m_labels[label] = address;
        } else {
            // 解析指令
            Instruction instr = parseInstruction(line);
            instr.address = address;
            m_instructions.push_back(instr);
            address += 4; // 简化：假设每条指令4字节
        }
    }

    LOG_INFO("Parsed " + std::to_string(m_instructions.size()) + " instructions");
    return true;
}

std::vector<AssemblyParser::Instruction> AssemblyParser::findInstructions(
    const std::string& mnemonic) {

    std::vector<Instruction> result;

    for (const auto& instr : m_instructions) {
        if (instr.mnemonic == mnemonic) {
            result.push_back(instr);
        }
    }

    return result;
}

std::vector<AssemblyParser::BasicBlock> AssemblyParser::getBasicBlocks() {
    std::vector<BasicBlock> blocks;

    // 简化实现：将所有指令作为一个基本块
    BasicBlock block;
    block.label = "entry";
    block.instructions = m_instructions;

    blocks.push_back(block);

    return blocks;
}

std::vector<size_t> AssemblyParser::findJumpTargets() {
    std::vector<size_t> targets;

    for (const auto& instr : m_instructions) {
        if (isJumpInstruction(instr.mnemonic)) {
            // 尝试从操作数中提取目标地址
            if (!instr.operands.empty()) {
                std::string target = instr.operands[0];

                // 如果是标签，查找对应地址
                if (m_labels.find(target) != m_labels.end()) {
                    targets.push_back(m_labels[target]);
                }
            }
        }
    }

    return targets;
}

std::vector<AssemblyParser::FunctionBoundary> AssemblyParser::findFunctionBoundaries() {
    std::vector<FunctionBoundary> boundaries;

    // 简化实现：查找标签作为函数边界
    for (const auto& [label, address] : m_labels) {
        FunctionBoundary boundary;
        boundary.name = label;
        boundary.startAddr = address;
        boundary.endAddr = address + 100; // 简化

        boundaries.push_back(boundary);
    }

    return boundaries;
}

AssemblyParser::Instruction AssemblyParser::parseInstruction(const std::string& line) {
    Instruction instr;

    std::istringstream iss(line);
    iss >> instr.mnemonic;

    // 解析操作数
    std::string operand;
    while (iss >> operand) {
        // 移除逗号
        if (!operand.empty() && operand.back() == ',') {
            operand.pop_back();
        }
        instr.operands.push_back(operand);
    }

    return instr;
}

bool AssemblyParser::isLabel(const std::string& line) {
    return line.find(':') != std::string::npos;
}

bool AssemblyParser::isJumpInstruction(const std::string& mnemonic) {
    static const std::vector<std::string> jumpInstructions = {
        "jmp", "je", "jne", "jz", "jnz", "jl", "jle", "jg", "jge",
        "ja", "jae", "jb", "jbe", "call", "ret"
    };

    return std::find(jumpInstructions.begin(), jumpInstructions.end(), mnemonic)
           != jumpInstructions.end();
}

// ============================================================================
// ASTBuilder Implementation
// ============================================================================

std::shared_ptr<CodeElement> ASTBuilder::build(const std::string& sourceCode) {
    auto root = std::make_shared<CodeElement>();
    root->type = CodeElementType::BLOCK;
    root->name = "root";
    root->content = sourceCode;
    root->startPos = 0;
    root->endPos = sourceCode.length();
    root->lineNumber = 1;

    // 简化实现：只创建根节点
    return root;
}

void ASTBuilder::traverse(const std::shared_ptr<CodeElement>& root, Visitor visitor) {
    if (!root) {
        return;
    }

    traverseImpl(root, visitor);
}

bool ASTBuilder::modifyNode(std::shared_ptr<CodeElement>& node,
                            const std::string& newContent) {
    if (!node) {
        return false;
    }

    node->content = newContent;
    return true;
}

bool ASTBuilder::insertNode(std::shared_ptr<CodeElement>& parent,
                            std::shared_ptr<CodeElement> newChild,
                            size_t position) {
    if (!parent || !newChild) {
        return false;
    }

    if (position > parent->children.size()) {
        return false;
    }

    parent->children.insert(parent->children.begin() + position, newChild);
    return true;
}

std::string ASTBuilder::generateCode(const std::shared_ptr<CodeElement>& root) {
    if (!root) {
        return "";
    }

    std::stringstream ss;
    ss << root->content;

    for (const auto& child : root->children) {
        ss << generateCode(child);
    }

    return ss.str();
}

std::shared_ptr<CodeElement> ASTBuilder::parseElement(const std::string& code, size_t& pos) {
    auto element = std::make_shared<CodeElement>();
    element->type = CodeElementType::UNKNOWN;
    element->content = code;
    element->startPos = pos;
    element->endPos = code.length();

    return element;
}

void ASTBuilder::traverseImpl(const std::shared_ptr<CodeElement>& node, Visitor& visitor) {
    if (!node) {
        return;
    }

    visitor(*node);

    for (const auto& child : node->children) {
        traverseImpl(child, visitor);
    }
}

} // namespace obfuscator
