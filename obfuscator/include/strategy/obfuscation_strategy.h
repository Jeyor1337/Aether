#ifndef OBFUSCATION_STRATEGY_H
#define OBFUSCATION_STRATEGY_H

#include <string>
#include <vector>
#include <memory>

namespace obfuscator {

// 混淆策略基类
class ObfuscationStrategy {
public:
    virtual ~ObfuscationStrategy() = default;

    // 应用混淆策略
    virtual bool apply(const std::string& input, std::string& output) = 0;

    // 获取策略名称
    virtual std::string getName() const = 0;

    // 获取策略描述
    virtual std::string getDescription() const = 0;

    // 设置混淆强度 (1-4)
    virtual void setLevel(int level) { m_level = level; }

    // 获取当前混淆强度
    int getLevel() const { return m_level; }

    // 启用/禁用策略
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

protected:
    int m_level = 2;        // 默认中等强度
    bool m_enabled = true;
};

// 垃圾指令插入策略
class JunkInstructionStrategy : public ObfuscationStrategy {
public:
    bool apply(const std::string& input, std::string& output) override;
    std::string getName() const override { return "JunkInstructions"; }
    std::string getDescription() const override {
        return "Insert meaningless but legal instructions";
    }

    // 设置垃圾指令密度
    void setDensity(float density) { m_density = density; }
    float getDensity() const { return m_density; }

    // 设置每个基本块最大垃圾指令数
    void setMaxPerBlock(int max) { m_maxPerBlock = max; }

private:
    float m_density = 0.3f;      // 垃圾指令密度
    int m_maxPerBlock = 5;       // 每个基本块最大垃圾指令数

    std::vector<std::string> generateJunkInstructions(int count);
};

// 控制流平坦化策略
class ControlFlowFlatteningStrategy : public ObfuscationStrategy {
public:
    bool apply(const std::string& input, std::string& output) override;
    std::string getName() const override { return "ControlFlowFlattening"; }
    std::string getDescription() const override {
        return "Flatten control flow using switch-case dispatcher";
    }

    void setFlattenDepth(int depth) { m_flattenDepth = depth; }
    void setAddFakeBranches(bool add) { m_addFakeBranches = add; }

private:
    int m_flattenDepth = 2;
    bool m_addFakeBranches = true;
};

// 不透明谓词策略
class OpaquePredicateStrategy : public ObfuscationStrategy {
public:
    bool apply(const std::string& input, std::string& output) override;
    std::string getName() const override { return "OpaquePredicates"; }
    std::string getDescription() const override {
        return "Insert predicates that are always true/false but hard to analyze";
    }

    enum class Complexity { LOW, MEDIUM, HIGH };
    void setComplexity(Complexity c) { m_complexity = c; }

private:
    Complexity m_complexity = Complexity::MEDIUM;

    std::string generateOpaquePredicate(bool alwaysTrue);
};

// 字符串加密策略
class StringEncryptionStrategy : public ObfuscationStrategy {
public:
    bool apply(const std::string& input, std::string& output) override;
    std::string getName() const override { return "StringEncryption"; }
    std::string getDescription() const override {
        return "Encrypt string literals and decrypt at runtime";
    }

    enum class Algorithm { XOR, AES, CUSTOM };
    void setAlgorithm(Algorithm algo) { m_algorithm = algo; }
    void setMinLength(int len) { m_minLength = len; }

private:
    Algorithm m_algorithm = Algorithm::XOR;
    int m_minLength = 4;

    std::string encryptString(const std::string& str, uint8_t key);
};

// 符号混淆策略
class SymbolObfuscationStrategy : public ObfuscationStrategy {
public:
    bool apply(const std::string& input, std::string& output) override;
    std::string getName() const override { return "SymbolObfuscation"; }
    std::string getDescription() const override {
        return "Rename functions and variables to meaningless names";
    }

private:
    std::string generateRandomName(int length = 8);
};

// 策略工厂
class StrategyFactory {
public:
    static std::unique_ptr<ObfuscationStrategy> createStrategy(const std::string& name);
    static std::vector<std::string> getAvailableStrategies();
};

} // namespace obfuscator

#endif // OBFUSCATION_STRATEGY_H
