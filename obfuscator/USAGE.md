# Code Obfuscator 使用指南

## 编译完成

混淆器已成功实现并编译。所有核心功能都已实现：

### 已实现的功能

1. **工具类**
   - `RandomUtils`: 随机数生成、随机字符串、加密工具
   - `Logger`: 完整的日志系统，支持多级别日志

2. **混淆策略**
   - `JunkInstructionStrategy`: 垃圾指令插入
   - `OpaquePredicateStrategy`: 不透明谓词插入
   - `StringEncryptionStrategy`: 字符串加密
   - `SymbolObfuscationStrategy`: 符号混淆
   - `ControlFlowFlatteningStrategy`: 控制流平坦化

3. **混淆引擎**
   - `ObfuscationEngine`: 核心混淆引擎
   - `InstrumentationEngine`: 代码插桩引擎
   - `ControlFlowRewriter`: 控制流重写器
   - `CodeValidator`: 代码验证器

4. **代码解析器**
   - `CodeParser`: C/C++代码解析
   - `AssemblyParser`: 汇编代码解析
   - `ASTBuilder`: 抽象语法树构建

## 使用方法

### 基本用法

```bash
cd obfuscator/build
./obfuscator-cli -i input.c -o output.c -l 2
```

### 参数说明

- `-i, --input <file>`: 输入源文件
- `-o, --output <file>`: 输出文件
- `-l, --level <1-4>`: 混淆等级
  - Level 1: 轻度混淆（垃圾指令）
  - Level 2: 中度混淆（垃圾指令 + 不透明谓词）
  - Level 3: 重度混淆（+ 字符串加密）
  - Level 4: 极限混淆（+ 控制流平坦化）
- `-v, --verbose`: 详细输出
- `-h, --help`: 显示帮助

### 测试结果

使用 `examples/simple_example.c` 测试（1.8K）：

| 等级 | 输出大小 | 膨胀率 | 应用策略 |
|------|---------|--------|---------|
| 1 | 2.9K | ~61% | 垃圾指令 |
| 2 | 3.8K | ~111% | 垃圾指令 + 不透明谓词 |
| 3 | 4.0K | ~122% | + 字符串加密 |
| 4 | 3.2K | ~78% | + 控制流平坦化 |

### 示例

#### Level 1 混淆（轻度）
```bash
./obfuscator-cli -i ../examples/simple_example.c -o output.c -l 1
```

效果：插入少量垃圾指令

#### Level 2 混淆（推荐）
```bash
./obfuscator-cli -i ../examples/simple_example.c -o output.c -l 2 -v
```

效果：
- 插入垃圾指令
- 添加不透明谓词（永真条件）
- 代码膨胀约100%

#### Level 3 混淆（重度）
```bash
./obfuscator-cli -i ../examples/simple_example.c -o output.c -l 3
```

效果：包含 Level 2 所有功能 + 字符串加密标记

#### Level 4 混淆（极限）
```bash
./obfuscator-cli -i ../examples/simple_example.c -o output.c -l 4
```

效果：包含所有混淆策略

## 混淆示例

### 原始代码
```c
int add(int a, int b) {
    return a + b;
}
```

### 混淆后（Level 2）
```c
int add(int a, int b) {
    return a + b;
    void* __ptr_tmp = (void*)0;
    __ptr_tmp = __ptr_tmp;
}
    int __junk_745 = 57;
    __junk_745 += 114;
    __junk_745 -= 114;
    for (volatile int __i = 0; __i < 0; __i++) {}
```

## 警告

⚠️ **本工具仅用于合法目的：**
- ✅ 保护自己的软件
- ✅ 安全研究和教育
- ✅ CTF竞赛
- ❌ 恶意软件开发
- ❌ 非法用途

## 下一步

如需更强的混淆效果，建议：
1. 编译时使用 `-O3 -fno-inline` 优化
2. 使用 LLVM Pass 进行 IR 级混淆（需要 LLVM 库）
3. 结合多次混淆
4. 使用代码虚拟化技术

## 技术细节

查看以下文档了解更多：
- `docs/TECHNICAL.md`: 技术实现细节
- `docs/DEVELOPMENT.md`: 开发指南
- `PROJECT_OVERVIEW.md`: 项目概览
