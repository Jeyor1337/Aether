# 开发指南

本文档面向想要参与项目开发或扩展混淆器功能的开发者。

## 目录

- [环境搭建](#环境搭建)
- [代码结构](#代码结构)
- [添加新的混淆策略](#添加新的混淆策略)
- [调试技巧](#调试技巧)
- [贡献指南](#贡献指南)

## 环境搭建

### 必需工具

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git

# macOS
brew install cmake

# Termux (Android)
pkg install clang cmake git
```

### 可选依赖

```bash
# LLVM开发库（用于IR级混淆）
sudo apt-get install llvm-dev libclang-dev

# Google Test（用于测试）
sudo apt-get install libgtest-dev
```

### 克隆和编译

```bash
git clone <repository-url>
cd obfuscator
./scripts/build.sh
```

## 代码结构

### 核心组件

```
include/
├── strategy/           # 混淆策略接口和实现
│   └── obfuscation_strategy.h
├── engine/            # 混淆引擎
│   └── obfuscation_engine.h
├── parser/            # 代码解析器
│   └── code_parser.h
└── utils/             # 工具类
    ├── config_manager.h
    ├── logger.h
    └── random_utils.h
```

### 关键类

#### ObfuscationStrategy（策略基类）

所有混淆策略继承此基类：

```cpp
class ObfuscationStrategy {
public:
    virtual bool apply(const std::string& input, std::string& output) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};
```

#### ObfuscationEngine（混淆引擎）

管理所有策略并执行混淆：

```cpp
class ObfuscationEngine {
public:
    void addStrategy(std::unique_ptr<ObfuscationStrategy> strategy);
    bool obfuscate(const std::string& input, std::string& output);
};
```

## 添加新的混淆策略

### 步骤1: 创建策略类

在 `include/strategy/` 创建新头文件：

```cpp
// my_strategy.h
#pragma once
#include "strategy/obfuscation_strategy.h"

class MyCustomStrategy : public ObfuscationStrategy {
public:
    bool apply(const std::string& input, std::string& output) override {
        // 实现混淆逻辑
        output = input;
        // ... 修改 output
        return true;
    }

    std::string getName() const override {
        return "MyCustomStrategy";
    }

    std::string getDescription() const override {
        return "Description of my strategy";
    }
};
```

### 步骤2: 实现策略

在 `src/strategy/` 创建实现文件：

```cpp
// my_strategy.cpp
#include "strategy/my_strategy.h"

bool MyCustomStrategy::apply(const std::string& input, std::string& output) {
    // 详细实现
    output = transformCode(input);
    return true;
}
```

### 步骤3: 注册策略

在 `StrategyFactory::createStrategy()` 中注册：

```cpp
if (name == "my_custom") {
    return std::make_unique<MyCustomStrategy>();
}
```

### 步骤4: 添加配置支持

在 `config.json` 中添加：

```json
{
  "strategies": {
    "my_custom_strategy": true
  }
}
```

### 步骤5: 编写测试

在 `tests/unit/` 创建测试文件：

```cpp
TEST(my_strategy_test) {
    MyCustomStrategy strategy;
    std::string input = "int main() { return 0; }";
    std::string output;

    bool result = strategy.apply(input, output);
    assert(result == true);
    assert(output != input);
}
```

## 调试技巧

### 1. 详细日志

使用 `-v` 参数启用详细输出：

```bash
./obfuscator-cli -i input.c -o output.c -v
```

### 2. 使用调试器

```bash
# 编译调试版本
./scripts/build.sh --debug

# 使用GDB
gdb ./build/obfuscator-cli
(gdb) run -i input.c -o output.c
(gdb) break obfuscate
```

### 3. 输出中间结果

在策略中添加调试输出：

```cpp
bool MyStrategy::apply(const std::string& input, std::string& output) {
    #ifdef DEBUG
    std::cout << "Input size: " << input.size() << std::endl;
    #endif

    // ... 处理代码

    #ifdef DEBUG
    std::cout << "Output size: " << output.size() << std::endl;
    #endif
}
```

### 4. 验证混淆正确性

```bash
# 编译原始代码
gcc input.c -o original

# 编译混淆代码
./obfuscator-cli -i input.c -o obf.c
gcc obf.c -o obfuscated

# 对比输出
./original > out1.txt
./obfuscated > out2.txt
diff out1.txt out2.txt  # 应该相同
```

## 混淆策略设计原则

### 1. 保持语义等价

混淆前后程序行为必须一致：

```cpp
// 好的混淆
int x = 5;
x = x + 10;
x = x - 10;  // x仍为5

// 坏的混淆
int x = 5;
x = x + 10;  // x变成15，破坏了语义
```

### 2. 控制性能影响

监控混淆引入的开销：

```cpp
struct Statistics {
    size_t originalSize;
    size_t obfuscatedSize;
    double sizeIncrease;
    double timeTaken;
};
```

### 3. 随机化

避免可预测的混淆模式：

```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 100);

int random_value = dis(gen);
```

### 4. 可配置性

允许用户调整混淆强度：

```cpp
void setDensity(float density);  // 0.0 - 1.0
void setComplexity(Complexity level);  // LOW, MEDIUM, HIGH
```

## 性能优化

### 1. 避免重复解析

缓存解析结果：

```cpp
class CodeParser {
private:
    std::map<std::string, ParsedResult> cache;
public:
    ParsedResult parse(const std::string& code) {
        if (cache.count(code)) {
            return cache[code];
        }
        // ... 解析
        cache[code] = result;
        return result;
    }
};
```

### 2. 并行处理

对独立函数并行混淆：

```cpp
#include <thread>
#include <future>

std::vector<std::future<std::string>> futures;
for (auto& func : functions) {
    futures.push_back(std::async(std::launch::async,
        [&]() { return obfuscateFunction(func); }));
}
```

## 测试最佳实践

### 1. 单元测试

测试每个策略独立工作：

```cpp
TEST(junk_instruction_test) {
    JunkInstructionStrategy strategy;
    strategy.setDensity(0.5);

    std::string input = loadTestFile("test.c");
    std::string output;

    ASSERT_TRUE(strategy.apply(input, output));
    ASSERT_GT(output.size(), input.size());
}
```

### 2. 集成测试

测试多个策略组合：

```cpp
TEST(combined_strategies_test) {
    ObfuscationEngine engine;
    engine.addStrategy(std::make_unique<JunkInstructionStrategy>());
    engine.addStrategy(std::make_unique<OpaquePredicateStrategy>());

    std::string input = "int add(int a, int b) { return a + b; }";
    std::string output;

    ASSERT_TRUE(engine.obfuscate(input, output));
    // 验证功能等价性
}
```

### 3. 基准测试

测量性能影响：

```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();
obfuscateCode(largeFile);
auto end = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Time: " << duration.count() << "ms" << std::endl;
```

## 贡献指南

### 提交代码前

1. **运行所有测试**
   ```bash
   cd build
   ctest --output-on-failure
   ```

2. **格式化代码**
   ```bash
   clang-format -i src/**/*.cpp include/**/*.h
   ```

3. **更新文档**
   - 更新 README.md（如果添加新功能）
   - 添加代码注释
   - 更新 CHANGELOG

### Pull Request规范

- 清晰的标题和描述
- 关联相关Issue
- 包含测试代码
- 通过所有CI检查

## 常见问题

### Q: 如何调试LLVM Pass？

```bash
# 编译时添加调试信息
cmake .. -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_ASSERTIONS=ON

# 使用opt运行Pass
opt -load build/lib/ObfuscationPass.so -junk-instr < input.ll > output.ll
```

### Q: 如何添加新的配置选项？

1. 在 `config.json` 添加键值
2. 在 `ConfigManager` 添加访问方法
3. 在策略中读取配置

### Q: 如何提高混淆强度？

- 增加垃圾指令密度
- 启用控制流平坦化
- 使用多层不透明谓词
- 启用字符串加密

## 参考资料

- [LLVM Pass开发文档](https://llvm.org/docs/WritingAnLLVMPass.html)
- [代码混淆技术综述](https://www.example.com)
- [反汇编工具对比](https://www.example.com)

## 联系方式

- Issues: GitHub Issues
- 讨论: GitHub Discussions
- Email: dev@example.com
