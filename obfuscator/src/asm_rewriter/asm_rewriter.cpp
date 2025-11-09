#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <random>

namespace obfuscator {

// 汇编重写器实现
class AsmRewriter {
public:
    AsmRewriter() : m_gen(std::random_device{}()) {}

    // 在汇编代码中插入垃圾指令
    std::string insertJunkInstructions(const std::string& asmCode, float density = 0.3f) {
        std::vector<std::string> lines = splitLines(asmCode);
        std::stringstream result;

        for (const auto& line : lines) {
            result << line << "\n";

            // 随机决定是否插入垃圾指令
            if (shouldInsertJunk(density) && !isLabel(line)) {
                result << generateJunkInstruction() << "\n";
            }
        }

        return result.str();
    }

    // 生成不透明谓词跳转
    std::string insertOpaqueJump(const std::string& asmCode) {
        std::stringstream result;
        result << "    xor eax, eax\n";         // eax = 0
        result << "    test eax, eax\n";        // test if 0
        result << "    jz .L_continue\n";       // always jump
        result << "    .byte 0xE8, 0x00, 0x00, 0x00, 0x00\n";  // fake call
        result << ".L_continue:\n";
        result << asmCode;
        return result.str();
    }

    // 添加栈平衡的垃圾指令
    std::string addStackJunk() {
        std::vector<std::string> templates = {
            "    push rax\n    pop rax",
            "    push rbx\n    pop rbx",
            "    push rcx\n    pop rcx",
            "    pushf\n    popf"
        };
        return templates[randomInt(0, templates.size() - 1)];
    }

private:
    std::mt19937 m_gen;

    std::vector<std::string> splitLines(const std::string& text) {
        std::vector<std::string> lines;
        std::stringstream ss(text);
        std::string line;
        while (std::getline(ss, line)) {
            lines.push_back(line);
        }
        return lines;
    }

    bool shouldInsertJunk(float density) {
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(m_gen) < density;
    }

    bool isLabel(const std::string& line) {
        return line.find(':') != std::string::npos && line.find_first_not_of(" \t") == 0;
    }

    std::string generateJunkInstruction() {
        std::vector<std::string> junkTemplates = {
            "    add rax, 0x10\n    sub rax, 0x10",
            "    xor ecx, ecx\n    add ecx, 1\n    sub ecx, 1",
            "    push rdx\n    pop rdx",
            "    nop\n    nop",
            "    xchg rax, rax",
            "    mov rbx, rbx",
            "    lea rax, [rax+0]"
        };

        return junkTemplates[randomInt(0, junkTemplates.size() - 1)];
    }

    int randomInt(int min, int max) {
        std::uniform_int_distribution<> dis(min, max);
        return dis(m_gen);
    }
};

} // namespace obfuscator
