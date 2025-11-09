#include "utils/config_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace obfuscator {

// 简单的JSON解析器（仅用于演示，生产环境应使用专业库如nlohmann/json）
class SimpleJsonParser {
public:
    static std::map<std::string, std::string> parse(const std::string& json) {
        std::map<std::string, std::string> result;
        // 简化的JSON解析 - 仅支持简单的key-value对
        // 生产环境应使用完整的JSON库
        return result;
    }
};

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() {
    setDefaultConfig();
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_filename = filename;

    return loadFromString(buffer.str());
}

bool ConfigManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << toJsonString();
    return true;
}

bool ConfigManager::loadFromString(const std::string& jsonString) {
    // 简化实现 - 实际应使用JSON库
    // 这里仅演示结构
    return true;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    auto it = m_config.find(key);
    if (it != m_config.end() && it->second.type == ConfigValue::Type::INT) {
        return it->second.intVal;
    }
    return defaultValue;
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    auto it = m_config.find(key);
    if (it != m_config.end() && it->second.type == ConfigValue::Type::DOUBLE) {
        return it->second.doubleVal;
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    auto it = m_config.find(key);
    if (it != m_config.end() && it->second.type == ConfigValue::Type::BOOL) {
        return it->second.boolVal;
    }
    return defaultValue;
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = m_config.find(key);
    if (it != m_config.end() && it->second.type == ConfigValue::Type::STRING) {
        return it->second.stringVal;
    }
    return defaultValue;
}

std::vector<std::string> ConfigManager::getStringArray(const std::string& key) const {
    auto it = m_config.find(key);
    if (it != m_config.end() && it->second.type == ConfigValue::Type::ARRAY) {
        return it->second.arrayVal;
    }
    return {};
}

std::vector<int> ConfigManager::getIntArray(const std::string& key) const {
    std::vector<int> result;
    auto strArray = getStringArray(key);
    for (const auto& str : strArray) {
        result.push_back(std::stoi(str));
    }
    return result;
}

void ConfigManager::setInt(const std::string& key, int value) {
    ConfigValue cv;
    cv.type = ConfigValue::Type::INT;
    cv.intVal = value;
    m_config[key] = cv;
}

void ConfigManager::setDouble(const std::string& key, double value) {
    ConfigValue cv;
    cv.type = ConfigValue::Type::DOUBLE;
    cv.doubleVal = value;
    m_config[key] = cv;
}

void ConfigManager::setBool(const std::string& key, bool value) {
    ConfigValue cv;
    cv.type = ConfigValue::Type::BOOL;
    cv.boolVal = value;
    m_config[key] = cv;
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    ConfigValue cv;
    cv.type = ConfigValue::Type::STRING;
    cv.stringVal = value;
    m_config[key] = cv;
}

bool ConfigManager::hasKey(const std::string& key) const {
    return m_config.find(key) != m_config.end();
}

bool ConfigManager::isStrategyEnabled(const std::string& strategy) const {
    std::string key = "strategies." + strategy;
    return getBool(key, false);
}

std::vector<std::string> ConfigManager::getTargetFunctions() const {
    return getStringArray("targets.functions");
}

std::vector<std::string> ConfigManager::getExcludedFunctions() const {
    return getStringArray("targets.exclude");
}

void ConfigManager::resetToDefaults() {
    m_config.clear();
    setDefaultConfig();
}

bool ConfigManager::validate() const {
    // 验证关键配置项
    int level = getObfuscationLevel();
    if (level < 1 || level > 4) {
        std::cerr << "Invalid obfuscation level: " << level << std::endl;
        return false;
    }

    int maxSize = getMaxCodeSizeIncrease();
    if (maxSize < 0 || maxSize > 100) {
        std::cerr << "Invalid max code size increase: " << maxSize << std::endl;
        return false;
    }

    return true;
}

void ConfigManager::print() const {
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Obfuscation Level: " << getObfuscationLevel() << std::endl;
    std::cout << "  Max Code Size Increase: " << getMaxCodeSizeIncrease() << "%" << std::endl;
    std::cout << "  Runtime Overhead: " << getAllowedRuntimeOverhead() << "%" << std::endl;
}

void ConfigManager::setDefaultConfig() {
    setInt("obfuscation_level", 2);
    setBool("strategies.junk_instructions", true);
    setBool("strategies.control_flow_flattening", true);
    setBool("strategies.opaque_predicates", true);
    setBool("strategies.string_encryption", true);
    setInt("performance.max_code_size_increase", 30);
    setInt("performance.allow_runtime_overhead", 15);
}

std::string ConfigManager::toJsonString() const {
    // 简化的JSON生成 - 实际应使用JSON库
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"obfuscation_level\": " << getObfuscationLevel() << ",\n";
    ss << "  \"performance\": {\n";
    ss << "    \"max_code_size_increase\": " << getMaxCodeSizeIncrease() << ",\n";
    ss << "    \"allow_runtime_overhead\": " << getAllowedRuntimeOverhead() << "\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

} // namespace obfuscator
