#include "Logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

const char* ToString(Logger::Level level) {
    switch (level) {
    case Logger::Level::Debug: return "DEBUG";
    case Logger::Level::Info: return "INFO";
    case Logger::Level::Warning: return "WARN";
    case Logger::Level::Error: return "ERROR";
    default: return "INFO";
    }
}

} // namespace

Logger& Logger::Instance() {
    static Logger logger;
    return logger;
}

Logger::Logger()
    : minLevel(Level::Info),
      mirrorToStdErr(true),
      maxEntries(200) {}

void Logger::SetMinLevel(Level level) {
    minLevel = level;
}

void Logger::SetMirrorToStdErr(bool enabled) {
    mirrorToStdErr = enabled;
}

bool Logger::IsMirrorToStdErrEnabled() const {
    return mirrorToStdErr;
}

void Logger::Debug(const std::string& message) {
    Log(Level::Debug, message);
}

void Logger::Info(const std::string& message) {
    Log(Level::Info, message);
}

void Logger::Warn(const std::string& message) {
    Log(Level::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(Level::Error, message);
}

const std::deque<std::string>& Logger::GetRecentEntries() const {
    return recentEntries;
}

void Logger::Log(Level level, const std::string& message) {
    if (static_cast<int>(level) < static_cast<int>(minLevel)) {
        return;
    }

    const std::string entry = Format(level, message);
    recentEntries.push_back(entry);
    while (recentEntries.size() > maxEntries) {
        recentEntries.pop_front();
    }

    if (mirrorToStdErr) {
        std::cerr << entry << '\n';
    }
}

std::string Logger::Format(Level level, const std::string& message) const {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

    std::tm tmValue{};
#if defined(_WIN32)
    localtime_s(&tmValue, &nowTime);
#else
    localtime_r(&nowTime, &tmValue);
#endif

    std::ostringstream os;
    os << '[' << std::put_time(&tmValue, "%H:%M:%S") << "] "
       << '[' << ToString(level) << "] "
       << message;
    return os.str();
}
