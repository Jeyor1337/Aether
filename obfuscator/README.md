# Code Obfuscator - 花指令混淆器

一个基于LLVM的C/C++代码混淆工具，通过插入花指令、控制流平坦化等技术提升代码的逆向分析难度。

## 重要声明

**本项目仅用于教育、安全研究和合法的软件保护用途。**

- 仅用于保护自己开发的合法软件
- 可用于安全研究和CTF竞赛
- 禁止用于恶意软件开发或非法用途
- 使用者需自行承担法律责任

## 功能特性

- **多种混淆策略**
  - 垃圾指令插入（Junk Instructions）
  - 控制流平坦化（Control Flow Flattening）
  - 不透明谓词（Opaque Predicates）
  - 字符串加密（String Encryption）
  - 符号混淆（Symbol Obfuscation）

- **灵活配置**
  - 4个混淆强度等级（轻度/中度/重度/极限）
  - JSON格式配置文件
  - 可选择性混淆特定函数
  - 性能和代码大小控制

- **双重工作模式**
  - LLVM IR级别混淆（编译时）
  - 汇编级别混淆（后处理）

## 项目结构

```
obfuscator/
├── include/              # 头文件
│   ├── parser/          # 代码解析器
│   ├── strategy/        # 混淆策略
│   ├── engine/          # 插桩引擎
│   └── utils/           # 工具函数
├── src/                 # 源代码
│   ├── llvm_pass/       # LLVM Pass实现
│   ├── asm_rewriter/    # 汇编重写器
│   ├── config/          # 配置管理
│   └── main.cpp         # CLI入口
├── templates/           # 花指令模板库
├── tests/               # 测试套件
├── examples/            # 示例程序
├── scripts/             # 自动化脚本
└── docs/                # 技术文档
```

## 快速开始

### 环境要求

- CMake 3.15+
- C++17 编译器（GCC 7+, Clang 6+）
- LLVM 10+ （可选，用于IR级混淆）
- Python 3.7+ （用于脚本）

### 编译

```bash
cd obfuscator
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 基础使用

```bash
# 使用默认配置混淆
./obfuscator-cli -i input.c -o output.c

# 使用自定义配置
./obfuscator-cli -i input.c -o output.c -c config.json

# 指定混淆等级
./obfuscator-cli -i input.c -o output.c --level 2
```

## 配置示例

创建 `config.json`：

```json
{
  "obfuscation_level": 2,
  "strategies": {
    "junk_instructions": true,
    "control_flow_flattening": true,
    "opaque_predicates": true,
    "string_encryption": true
  },
  "targets": {
    "functions": ["encrypt_*", "validate_*"],
    "exclude": ["debug_*", "test_*"]
  },
  "performance": {
    "max_code_size_increase": 30,
    "allow_runtime_overhead": 15
  }
}
```

## 混淆效果示例

**混淆前：**
```c
int add(int a, int b) {
    return a + b;
}
```

**混淆后（Level 2）：**
```c
int add(int a, int b) {
    int result;
    __asm__ volatile (
        "push %%rax\n"
        "pop %%rax\n"           // 垃圾指令
        "jz 1f\n"
        "jnz 1f\n"              // 永恒跳转
        ".byte 0xE8\n"          // 反汇编器陷阱
        "1:\n"
        "mov %1, %%eax\n"
        "add %2, %%eax\n"
        "mov %%eax, %0\n"
        : "=r"(result)
        : "r"(a), "r"(b)
        : "rax"
    );
    return result;
}
```

## 开发路线图

- [x] 项目架构设计
- [x] 基础框架搭建
- [ ] LLVM Pass实现
- [ ] 汇编级混淆器
- [ ] 控制流平坦化
- [ ] 字符串加密
- [ ] 虚拟机混淆（计划中）
- [ ] GUI界面（计划中）

## 技术栈

- **C++17**: 核心引擎
- **LLVM/Clang**: IR级别分析与变换
- **Capstone**: 反汇编引擎
- **Keystone**: 汇编引擎
- **Python**: 自动化脚本

## 测试

```bash
# 运行所有测试
cd build
ctest --output-on-failure

# 运行性能基准测试
./tests/benchmark/benchmark_runner
```

## 参考资料

- [Obfuscator-LLVM](https://github.com/obfuscator-llvm/obfuscator)
- [LLVM Pass开发指南](https://llvm.org/docs/WritingAnLLVMPass.html)
- "Code Obfuscation Techniques" - Collberg et al.

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

## 贡献

欢迎提交Issue和Pull Request！

## 免责声明

本软件按"原样"提供，不提供任何明示或暗示的保证。作者不对因使用本软件造成的任何损害承担责任。使用者需确保遵守所在地区的法律法规。

**警告**: 代码混淆不能替代加密和其他安全措施，仅作为额外的防护层。
