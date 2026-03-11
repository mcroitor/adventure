#include "GameConfig.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace {

std::string Trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::vector<std::string> Split(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        part = Trim(part);
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    return parts;
}

bool TryParseInt(const std::string& value, int& parsed) {
    try {
        size_t consumed = 0;
        parsed = std::stoi(value, &consumed);
        return consumed == value.size();
    } catch (...) {
        return false;
    }
}

} // namespace

GameConfig& GameConfig::Instance() {
    static GameConfig config;
    return config;
}

GameConfig::GameConfig()
    : messageLogCapacity(24),
      autoSaveIntervalSeconds(45),
      monsterSpawnIntervalSeconds(20),
      savePath("map_save.txt"),
      questTemplatePaths({"data/quests.txt", "../data/quests.txt"}) {}

bool GameConfig::LoadFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(input, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        std::string key = Trim(line.substr(0, separator));
        std::string value = Trim(line.substr(separator + 1));
        if (key.empty() || value.empty()) {
            continue;
        }

        if (key == "message_log_capacity") {
            int parsed = 0;
            if (TryParseInt(value, parsed)) {
                messageLogCapacity = std::max(1, parsed);
            }
            continue;
        }

        if (key == "auto_save_interval_seconds") {
            int parsed = 0;
            if (TryParseInt(value, parsed)) {
                autoSaveIntervalSeconds = std::max(1, parsed);
            }
            continue;
        }

        if (key == "monster_spawn_interval_seconds") {
            int parsed = 0;
            if (TryParseInt(value, parsed)) {
                monsterSpawnIntervalSeconds = std::max(1, parsed);
            }
            continue;
        }

        if (key == "save_path") {
            savePath = value;
            continue;
        }

        if (key == "quest_template_paths") {
            auto parsed = Split(value, ';');
            if (!parsed.empty()) {
                questTemplatePaths = parsed;
            }
        }
    }

    return true;
}

int GameConfig::GetMessageLogCapacity() const {
    return messageLogCapacity;
}

int GameConfig::GetAutoSaveIntervalSeconds() const {
    return autoSaveIntervalSeconds;
}

int GameConfig::GetMonsterSpawnIntervalSeconds() const {
    return monsterSpawnIntervalSeconds;
}

const std::string& GameConfig::GetSavePath() const {
    return savePath;
}

const std::vector<std::string>& GameConfig::GetQuestTemplatePaths() const {
    return questTemplatePaths;
}
