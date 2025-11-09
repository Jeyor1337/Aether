#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace obfuscator {

// 配置管理器
class ConfigManager {
public:
    // 单例模式
    static ConfigManager& getInstance();

    // 从JSON文件加载配置
    bool loadFromFile(const std::string& filename);

    // 保存配置到JSON文件
    bool saveToFile(const std::string& filename);

    // 从JSON字符串加载
    bool loadFromString(const std::string& jsonString);

    // 获取配置项
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;

    // 获取数组配置
    std::vector<std::string> getStringArray(const std::string& key) const;
    std::vector<int> getIntArray(const std::string& key) const;

    // 设置配置项
    void setInt(const std::string& key, int value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    void setString(const std::string& key, const std::string& value);

    // 检查配置项是否存在
    bool hasKey(const std::string& key) const;

    // 混淆配置快捷访问
    int getObfuscationLevel() const { return getInt("obfuscation_level", 2); }
    bool isStrategyEnabled(const std::string& strategy) const;
    std::vector<std::string> getTargetFunctions() const;
    std::vector<std::string> getExcludedFunctions() const;
    int getMaxCodeSizeIncrease() const { return getInt("performance.max_code_size_increase", 30); }
    int getAllowedRuntimeOverhead() const { return getInt("performance.allow_runtime_overhead", 15); }

    // 重置为默认配置
    void resetToDefaults();

    // 验证配置
    bool validate() const;

    // 打印配置（用于调试）
    void print() const;

private:
    ConfigManager();
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    struct ConfigValue {
        enum class Type { INT, DOUBLE, BOOL, STRING, ARRAY };
        Type type;
        union {
            int intVal;
            double doubleVal;
            bool boolVal;
        };
        std::string stringVal;
        std::vector<std::string> arrayVal;
    };

    std::map<std::string, ConfigValue> m_config;
    std::string m_filename;

    void setDefaultConfig();
    std::string toJsonString() const;
};

} // namespace obfuscator

#endif // CONFIG_MANAGER_H
