# 示例程序

这个目录包含用于测试混淆器的示例程序。

## 文件列表

### 1. simple_example.c
基础示例，包含：
- 简单的数学运算函数
- 条件分支
- 循环结构
- 字符串操作

**编译和运行：**
```bash
# 原始版本
gcc simple_example.c -o simple_example
./simple_example

# 混淆版本
../build/obfuscator-cli -i simple_example.c -o simple_example_obf.c -l 2
gcc simple_example_obf.c -o simple_example_obf
./simple_example_obf
```

### 2. password_checker.c
密码验证器示例，演示：
- 密码验证逻辑（适合混淆保护）
- 字符串比较
- 多重条件判断

**使用场景：**
- 学习如何保护敏感验证逻辑
- 测试字符串加密混淆
- 演示控制流平坦化效果

**编译和测试：**
```bash
# 编译原始版本
gcc password_checker.c -o password_checker
echo "SecretPass123" | ./password_checker

# 混淆版本
../build/obfuscator-cli -i password_checker.c -o password_checker_obf.c -l 3
gcc password_checker_obf.c -o password_checker_obf
echo "SecretPass123" | ./password_checker_obf
```

## 混淆等级建议

- **Level 1 (轻度)**: 适用于simple_example.c，轻微保护
- **Level 2 (中度)**: 通用保护，平衡性能和安全
- **Level 3 (重度)**: 适用于password_checker.c，保护敏感逻辑
- **Level 4 (极限)**: 最高保护，但可能影响性能

## 分析混淆效果

### 使用objdump查看汇编
```bash
# 原始程序
objdump -d simple_example > original.asm

# 混淆后
objdump -d simple_example_obf > obfuscated.asm

# 比较差异
diff original.asm obfuscated.asm
```

### 使用IDA Pro/Ghidra分析
1. 打开原始和混淆后的二进制文件
2. 对比控制流图（CFG）
3. 观察混淆效果：
   - 垃圾指令的插入位置
   - 控制流的复杂度变化
   - 字符串是否被加密

### 性能测试
```bash
# 使用time命令测试执行时间
time ./simple_example
time ./simple_example_obf

# 比较二进制大小
ls -lh simple_example simple_example_obf
```

## 预期混淆效果

### simple_example.c (Level 2)
- **代码增长**: 约25-30%
- **性能影响**: <10%
- **混淆技术**:
  - 垃圾指令插入
  - 不透明谓词
  - 字符串加密

### password_checker.c (Level 3)
- **代码增长**: 约40-50%
- **性能影响**: 10-15%
- **混淆技术**:
  - 控制流平坦化
  - 强字符串加密
  - 多层不透明谓词
  - 符号混淆

## 注意事项

1. **仅用于教育目的**：这些示例仅用于学习代码混淆技术
2. **不保证绝对安全**：混淆不能替代加密和安全设计
3. **性能考虑**：高混淆等级会影响执行速度
4. **调试困难**：混淆后的代码难以调试，保留原始版本

## 自定义配置

创建 `custom_config.json`：
```json
{
  "obfuscation_level": 2,
  "strategies": {
    "junk_instructions": true,
    "control_flow_flattening": false,
    "string_encryption": true
  },
  "targets": {
    "functions": ["validate_password", "check_password_strength"]
  }
}
```

使用自定义配置：
```bash
../build/obfuscator-cli -i password_checker.c -o output.c -c custom_config.json
```

## 下一步

1. 尝试混淆这些示例
2. 分析混淆前后的差异
3. 编写自己的测试程序
4. 调整配置文件优化混淆效果
5. 使用逆向工具验证保护强度
