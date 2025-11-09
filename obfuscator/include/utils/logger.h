#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <mutex>

namespace obfuscator {
namespace utils {

// 日志级别
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// 日志记录器
class Logger {
public:
    // 单例模式
    static Logger& getInstance();

    // 设置日志级别
    void setLogLevel(LogLevel level) { m_logLevel = level; }

    // 设置日志文件
    void setLogFile(const std::string& filename);

    // 启用/禁用控制台输出
    void setConsoleOutput(bool enable) { m_consoleOutput = enable; }

    // 启用/禁用文件输出
    void setFileOutput(bool enable) { m_fileOutput = enable; }

    // 日志记录方法
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

    // 带格式化的日志
    template<typename... Args>
    void debugf(const char* format, Args... args) {
        log(LogLevel::DEBUG, formatString(format, args...));
    }

    template<typename... Args>
    void infof(const char* format, Args... args) {
        log(LogLevel::INFO, formatString(format, args...));
    }

    template<typename... Args>
    void warningf(const char* format, Args... args) {
        log(LogLevel::WARNING, formatString(format, args...));
    }

    template<typename... Args>
    void errorf(const char* format, Args... args) {
        log(LogLevel::ERROR, formatString(format, args...));
    }

    // 刷新日志
    void flush();

    // 关闭日志文件
    void close();

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& message);
    std::string levelToString(LogLevel level) const;
    std::string getCurrentTime() const;

    template<typename... Args>
    std::string formatString(const char* format, Args... args) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), format, args...);
        return std::string(buffer);
    }

    LogLevel m_logLevel;
    bool m_consoleOutput;
    bool m_fileOutput;
    std::ofstream m_logFile;
    std::mutex m_mutex;
};

// 便捷宏
#define LOG_DEBUG(msg) obfuscator::utils::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) obfuscator::utils::Logger::getInstance().info(msg)
#define LOG_WARNING(msg) obfuscator::utils::Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) obfuscator::utils::Logger::getInstance().error(msg)
#define LOG_CRITICAL(msg) obfuscator::utils::Logger::getInstance().critical(msg)

} // namespace utils
} // namespace obfuscator

#endif // LOGGER_H
