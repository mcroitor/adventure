#pragma once

#include <cstddef>
#include <deque>
#include <string>

class Logger {
public:
    enum class Level {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3
    };

    static Logger& Instance();

    void SetMinLevel(Level level);
    void SetMirrorToStdErr(bool enabled);
    bool IsMirrorToStdErrEnabled() const;

    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warn(const std::string& message);
    void Error(const std::string& message);

    const std::deque<std::string>& GetRecentEntries() const;

private:
    Logger();

    void Log(Level level, const std::string& message);
    std::string Format(Level level, const std::string& message) const;

    Level minLevel;
    bool mirrorToStdErr;
    size_t maxEntries;
    std::deque<std::string> recentEntries;
};
