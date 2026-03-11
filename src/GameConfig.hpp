#pragma once

#include <string>
#include <vector>

class GameConfig {
public:
    static GameConfig& Instance();

    bool LoadFromFile(const std::string& path);

    int GetMessageLogCapacity() const;
    int GetAutoSaveIntervalSeconds() const;
    int GetMonsterSpawnIntervalSeconds() const;
    const std::string& GetSavePath() const;
    const std::vector<std::string>& GetQuestTemplatePaths() const;

private:
    GameConfig();

    int messageLogCapacity;
    int autoSaveIntervalSeconds;
    int monsterSpawnIntervalSeconds;
    std::string savePath;
    std::vector<std::string> questTemplatePaths;
};
