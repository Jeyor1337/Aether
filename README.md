# Aether

代码混淆器工具集合

## 项目结构

```
Aether/
├── obfuscator/          # C/C++ 代码混淆器
│   ├── src/            # 源代码
│   ├── include/        # 头文件
│   ├── docs/           # 技术文档
│   ├── examples/       # 示例代码
│   ├── tests/          # 测试套件
│   └── README.md       # 详细说明
└── README.md           # 本文件
```

## 子项目

### obfuscator - 花指令混淆器

基于 LLVM 的 C/C++ 代码混淆工具，提供多种混淆策略。

详细信息请查看 [obfuscator/README.md](obfuscator/README.md)

## 快速开始

```bash
cd obfuscator
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 许可证

MIT License

## 免责声明

本项目仅用于教育、安全研究和合法的软件保护用途。使用者需自行承担法律责任。
