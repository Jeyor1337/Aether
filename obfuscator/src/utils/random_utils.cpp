#include "utils/random_utils.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace obfuscator {
namespace utils {

// ============================================================================
// RandomGenerator Implementation
// ============================================================================

RandomGenerator::RandomGenerator()
    : m_engine(std::random_device{}()),
      m_doubleDist(0.0, 1.0) {
}

RandomGenerator& RandomGenerator::getInstance() {
    static RandomGenerator instance;
    return instance;
}

void RandomGenerator::setSeed(uint32_t seed) {
    m_engine.seed(seed);
}

int RandomGenerator::randomInt(int min, int max) {
    if (min > max) {
        std::swap(min, max);
    }
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_engine);
}

double RandomGenerator::randomDouble() {
    return m_doubleDist(m_engine);
}

bool RandomGenerator::randomBool(double probability) {
    return randomDouble() < probability;
}

std::vector<uint8_t> RandomGenerator::randomBytes(size_t count) {
    std::vector<uint8_t> bytes;
    bytes.reserve(count);

    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < count; ++i) {
        bytes.push_back(static_cast<uint8_t>(dist(m_engine)));
    }

    return bytes;
}

std::string RandomGenerator::randomString(size_t length) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    static const size_t charsetSize = sizeof(charset) - 1;

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += charset[randomInt(0, charsetSize - 1)];
    }

    return result;
}

std::string RandomGenerator::randomHexString(size_t length) {
    static const char hexChars[] = "0123456789abcdef";

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += hexChars[randomInt(0, 15)];
    }

    return result;
}

// ============================================================================
// NameGenerator Implementation
// ============================================================================

const std::vector<std::string> NameGenerator::s_prefixes = {
    "tmp", "var", "val", "ref", "ptr", "obj", "data", "buff",
    "mem", "sys", "fn", "func", "proc", "calc", "comp", "exec"
};

const std::vector<std::string> NameGenerator::s_suffixes = {
    "1", "2", "x", "y", "z", "a", "b", "ex", "tmp", "buf"
};

const std::vector<std::string> NameGenerator::s_words = {
    "alpha", "beta", "gamma", "delta", "theta", "omega",
    "prime", "main", "core", "base", "meta", "proto"
};

std::string NameGenerator::generateFunctionName(size_t length) {
    auto& rng = RandomGenerator::getInstance();

    if (rng.randomBool(0.5)) {
        // 格式: prefix_randomstring
        std::string prefix = rng.randomChoice(s_prefixes);
        std::string suffix = rng.randomString(length - prefix.length() - 1);
        return prefix + "_" + suffix;
    } else {
        // 纯随机字母
        std::string name;
        name += static_cast<char>('a' + rng.randomInt(0, 25)); // 首字母
        name += rng.randomString(length - 1);
        return name;
    }
}

std::string NameGenerator::generateVariableName(size_t length) {
    auto& rng = RandomGenerator::getInstance();

    if (rng.randomBool(0.3)) {
        // 短变量名: x, y, tmp等
        static const std::vector<std::string> shortNames = {
            "i", "j", "k", "x", "y", "z", "tmp", "val", "res"
        };
        return rng.randomChoice(shortNames) + rng.randomString(rng.randomInt(0, 2));
    } else {
        // 随机变量名
        std::string name;
        name += static_cast<char>('a' + rng.randomInt(0, 25));
        name += rng.randomString(length - 1);
        return name;
    }
}

std::string NameGenerator::generateMeaninglessName() {
    auto& rng = RandomGenerator::getInstance();

    std::string word1 = rng.randomChoice(s_words);
    std::string word2 = rng.randomChoice(s_words);

    return word1 + "_" + word2 + "_" + std::to_string(rng.randomInt(0, 999));
}

std::string NameGenerator::generateHexName() {
    auto& rng = RandomGenerator::getInstance();

    std::stringstream ss;
    ss << "_0x" << std::hex << std::setw(4) << std::setfill('0')
       << rng.randomInt(0x1000, 0xFFFF);

    return ss.str();
}

bool NameGenerator::isNameUnique(const std::string& name,
                                 const std::vector<std::string>& existingNames) {
    return std::find(existingNames.begin(), existingNames.end(), name)
           == existingNames.end();
}

// ============================================================================
// CryptoUtils Implementation
// ============================================================================

std::string CryptoUtils::xorEncrypt(const std::string& data, uint8_t key) {
    std::string result = data;
    for (char& c : result) {
        c ^= key;
    }
    return result;
}

std::string CryptoUtils::xorEncrypt(const std::string& data,
                                    const std::vector<uint8_t>& key) {
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }

    std::string result = data;
    size_t keyLen = key.size();

    for (size_t i = 0; i < result.size(); ++i) {
        result[i] ^= key[i % keyLen];
    }

    return result;
}

uint8_t CryptoUtils::generateKey8() {
    auto& rng = RandomGenerator::getInstance();
    return static_cast<uint8_t>(rng.randomInt(1, 255));
}

std::vector<uint8_t> CryptoUtils::generateKeyN(size_t length) {
    return RandomGenerator::getInstance().randomBytes(length);
}

std::string CryptoUtils::substitutionEncrypt(const std::string& data, int shift) {
    std::string result = data;

    for (char& c : result) {
        if (c >= 'a' && c <= 'z') {
            c = 'a' + (c - 'a' + shift) % 26;
        } else if (c >= 'A' && c <= 'Z') {
            c = 'A' + (c - 'A' + shift) % 26;
        }
    }

    return result;
}

std::string CryptoUtils::generateDecryptionCode(const std::string& encryptedData,
                                                uint8_t key,
                                                const std::string& varName) {
    std::stringstream ss;

    // 生成加密后的字节数组
    ss << "unsigned char " << varName << "_enc[] = {";
    for (size_t i = 0; i < encryptedData.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0')
           << (static_cast<unsigned int>(static_cast<uint8_t>(encryptedData[i])));
    }
    ss << "};\n";

    // 生成解密代码
    ss << "char " << varName << "[" << (encryptedData.size() + 1) << "];\n";
    ss << "for (int i = 0; i < " << std::dec << encryptedData.size() << "; i++) {\n";
    ss << "    " << varName << "[i] = " << varName << "_enc[i] ^ 0x"
       << std::hex << static_cast<int>(key) << ";\n";
    ss << "}\n";
    ss << varName << "[" << std::dec << encryptedData.size() << "] = '\\0';\n";

    return ss.str();
}

} // namespace utils
} // namespace obfuscator
