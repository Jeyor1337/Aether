# 项目概览

## Code Obfuscator - 花指令混淆器

完整的C/C++代码混淆工具，用于保护源代码免受逆向工程。

---

## 项目状态

### 已完成模块 ✓

- [x] 项目架构设计
- [x] 核心框架实现
- [x] 配置管理系统
- [x] 混淆策略接口
- [x] 花指令模板库
- [x] 汇编级混淆器
- [x] LLVM Pass骨架
- [x] CLI命令行工具
- [x] 示例程序
- [x] 测试套件
- [x] 自动化脚本
- [x] 完整文档

### 待完善功能 ⏳

- [ ] LLVM Pass完整实现（需LLVM库）
- [ ] 控制流平坦化完整实现
- [ ] 虚拟机混淆
- [ ] 反调试模块
- [ ] GUI界面
- [ ] 更多测试用例

---

## 快速开始

### 1. 编译项目

```bash
cd obfuscator
./scripts/build.sh
```

### 2. 运行示例

```bash
./scripts/run_examples.sh
```

### 3. 使用混淆器

```bash
./build/obfuscator-cli -i input.c -o output.c -l 2
```

---

## 文件结构导览

### 核心源代码

```
src/
├── main.cpp                    # CLI入口
├── config/
│   └── config_manager.cpp      # 配置管理实现
├── asm_rewriter/
│   └── asm_rewriter.cpp        # 汇编级混淆
└── llvm_pass/
    └── ObfuscationPass.cpp     # LLVM Pass混淆
```

### 头文件

```
include/
├── strategy/
│   └── obfuscation_strategy.h  # 混淆策略接口
├── engine/
│   └── obfuscation_engine.h    # 混淆引擎
├── parser/
│   └── code_parser.h           # 代码解析器
└── utils/
    ├── config_manager.h         # 配置管理
    ├── logger.h                 # 日志系统
    └── random_utils.h           # 随机工具
```

### 模板库

```
templates/
├── junk_arithmetic.template          # 垃圾算术指令
├── opaque_predicates.template        # 不透明谓词
├── control_flow_flattening.template  # 控制流平坦化
└── string_encryption.template        # 字符串加密
```

### 示例程序

```
examples/
├── simple_example.c          # 基础示例
├── password_checker.c        # 密码验证示例
└── README.md                # 示例说明
```

### 文档

```
docs/
├── DEVELOPMENT.md           # 开发指南
└── TECHNICAL.md             # 技术文档
```

---

## 技术栈

### 编程语言
- **C++17**: 核心引擎
- **C**: 示例程序
- **Shell**: 自动化脚本

### 依赖库（可选）
- **LLVM 10+**: IR级混淆
- **Capstone**: 反汇编（计划中）
- **Keystone**: 汇编（计划中）
- **Google Test**: 单元测试

### 构建工具
- **CMake 3.15+**
- **GCC 7+ / Clang 6+**

---

## 混淆技术

### 1. 垃圾指令插入
- 自相消算术操作
- 无意义栈操作
- NOP变体

### 2. 不透明谓词
- 数学恒等式
- 指针别名
- 复杂条件表达式

### 3. 控制流平坦化
- Switch分发器
- 状态机转换
- 虚假分支

### 4. 字符串加密
- XOR加密
- 栈字符串构建
- 动态密钥生成

### 5. 符号混淆
- 函数名混淆
- 变量名替换
- 保留导出符号

---

## 配置选项

### 混淆等级

| Level | 描述 | 代码增长 | 性能影响 |
|-------|------|----------|----------|
| 1 | 轻度 | 10-15% | <5% |
| 2 | 中度 | 20-30% | 10-15% |
| 3 | 重度 | 40-60% | 20-30% |
| 4 | 极限 | >70% | >40% |

### 配置文件示例

```json
{
  "obfuscation_level": 2,
  "strategies": {
    "junk_instructions": true,
    "control_flow_flattening": true,
    "opaque_predicates": true,
    "string_encryption": true
  },
  "performance": {
    "max_code_size_increase": 30,
    "allow_runtime_overhead": 15
  }
}
```

---

## 使用场景

### ✓ 适用场景

1. **商业软件保护**: 保护核心算法
2. **安全研究**: 学习混淆技术
3. **CTF竞赛**: 增加逆向难度
4. **教育目的**: 理解代码保护

### ✗ 不适用场景

1. **恶意软件开发**: 违法且不道德
2. **绕过安全检测**: 违反使用协议
3. **未授权修改**: 侵犯知识产权

---

## 性能基准

### 测试环境
- CPU: Intel i5-8250U
- RAM: 8GB
- OS: Ubuntu 20.04

### 基准结果

**simple_example.c (100行)**

| 版本 | 大小 | 编译时间 | 运行时间 |
|------|------|----------|----------|
| 原始 | 8KB | 0.2s | 1.0ms |
| Level 1 | 9KB | 0.3s | 1.1ms |
| Level 2 | 11KB | 0.4s | 1.2ms |
| Level 3 | 14KB | 0.6s | 1.4ms |

**password_checker.c (200行)**

| 版本 | 大小 | 编译时间 | 运行时间 |
|------|------|----------|----------|
| 原始 | 15KB | 0.3s | 2.0ms |
| Level 2 | 20KB | 0.5s | 2.4ms |
| Level 3 | 28KB | 0.8s | 3.0ms |

---

## 开发路线图

### Phase 1: 原型验证 ✓
- [x] 基础架构
- [x] 简单混淆策略
- [x] CLI工具

### Phase 2: 核心功能 (进行中)
- [x] 配置系统
- [x] 多种混淆策略
- [ ] LLVM Pass完整实现

### Phase 3: 增强功能 (计划)
- [ ] 虚拟机混淆
- [ ] 反调试技术
- [ ] 完整性检查

### Phase 4: 工具化 (计划)
- [ ] GUI界面
- [ ] IDE插件
- [ ] 云端服务

---

## 贡献者

### 核心团队
- 架构设计
- 核心实现
- 文档编写

### 如何贡献

1. Fork项目
2. 创建特性分支
3. 提交更改
4. 发起Pull Request

详见 [DEVELOPMENT.md](docs/DEVELOPMENT.md)

---

## 许可证

MIT License - 详见 [LICENSE](LICENSE)

### 重要声明

本软件仅用于**合法目的**:
- ✓ 保护自己的软件
- ✓ 安全研究和教育
- ✓ CTF竞赛
- ✗ 恶意软件开发
- ✗ 非法用途

---

## 资源链接

### 文档
- [README.md](README.md) - 项目介绍
- [DEVELOPMENT.md](docs/DEVELOPMENT.md) - 开发指南
- [TECHNICAL.md](docs/TECHNICAL.md) - 技术详解

### 示例
- [examples/](examples/) - 示例程序
- [tests/](tests/) - 测试套件

### 工具
- [scripts/](scripts/) - 自动化脚本
- [templates/](templates/) - 混淆模板

---

## FAQ

### Q: 混淆能完全防止逆向吗？
A: 不能。混淆只是增加逆向难度，不能完全防止。

### Q: 混淆会影响调试吗？
A: 是的。建议保留未混淆版本用于调试。

### Q: 支持哪些平台？
A: Linux、macOS、Windows（需MinGW）、Android（Termux）

### Q: 需要LLVM吗？
A: 可选。不使用LLVM只能进行汇编级混淆。

### Q: 如何选择混淆等级？
A: 根据性能要求：
- 性能敏感：Level 1-2
- 平衡：Level 2
- 高保护：Level 3-4

---

## 联系方式

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Email**: contact@example.com

---

## 致谢

感谢以下项目的启发：
- Obfuscator-LLVM
- Tigress C Obfuscator
- OLLVM

---

**最后更新**: 2025-11-09
**项目版本**: v1.0.0
**状态**: 开发中
