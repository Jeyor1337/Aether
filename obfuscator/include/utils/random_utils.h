#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <string>
#include <vector>
#include <random>
#include <cstdint>

namespace obfuscator {
namespace utils {

// 随机数生成器
class RandomGenerator {
public:
    // 单例模式
    static RandomGenerator& getInstance();

    // 设置随机种子（用于可重现的混淆）
    void setSeed(uint32_t seed);

    // 生成随机整数 [min, max]
    int randomInt(int min, int max);

    // 生成随机浮点数 [0.0, 1.0]
    double randomDouble();

    // 生成随机布尔值
    bool randomBool(double probability = 0.5);

    // 从数组中随机选择
    template<typename T>
    T randomChoice(const std::vector<T>& choices) {
        if (choices.empty()) {
            throw std::invalid_argument("Cannot choose from empty vector");
        }
        int index = randomInt(0, choices.size() - 1);
        return choices[index];
    }

    // 随机打乱数组
    template<typename T>
    void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), m_engine);
    }

    // 生成随机字节序列
    std::vector<uint8_t> randomBytes(size_t count);

    // 生成随机字符串（字母数字）
    std::string randomString(size_t length);

    // 生成随机十六进制字符串
    std::string randomHexString(size_t length);

private:
    RandomGenerator();
    ~RandomGenerator() = default;
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;

    std::mt19937 m_engine;
    std::uniform_real_distribution<double> m_doubleDist;
};

// 名称生成器（用于符号混淆）
class NameGenerator {
public:
    // 生成混淆后的函数名
    static std::string generateFunctionName(size_t length = 8);

    // 生成混淆后的变量名
    static std::string generateVariableName(size_t length = 6);

    // 生成看起来有意义但无实际意义的名称
    static std::string generateMeaninglessName();

    // 生成特定模式的名称（如_0x1234形式）
    static std::string generateHexName();

    // 确保名称唯一性
    static bool isNameUnique(const std::string& name,
                            const std::vector<std::string>& existingNames);

private:
    static const std::vector<std::string> s_prefixes;
    static const std::vector<std::string> s_suffixes;
    static const std::vector<std::string> s_words;
};

// 加密工具
class CryptoUtils {
public:
    // XOR加密
    static std::string xorEncrypt(const std::string& data, uint8_t key);
    static std::string xorEncrypt(const std::string& data, const std::vector<uint8_t>& key);

    // 生成加密密钥
    static uint8_t generateKey8();
    static std::vector<uint8_t> generateKeyN(size_t length);

    // 简单的替换加密（Caesar cipher变体）
    static std::string substitutionEncrypt(const std::string& data, int shift);

    // 生成解密代码片段（用于运行时解密）
    static std::string generateDecryptionCode(const std::string& encryptedData,
                                             uint8_t key,
                                             const std::string& varName);
};

} // namespace utils
} // namespace obfuscator

#endif // RANDOM_UTILS_H
