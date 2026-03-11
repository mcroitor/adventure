#include "ResourceManager.hpp"

#include <filesystem>

ResourceManager& ResourceManager::Instance() {
    static ResourceManager manager;
    return manager;
}

bool ResourceManager::FileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

bool ResourceManager::OpenFirstFound(
    const std::vector<std::string>& paths,
    std::ifstream& stream,
    std::string& resolvedPath
) const {
    for (const auto& path : paths) {
        std::ifstream candidate(path);
        if (!candidate.is_open()) {
            continue;
        }

        stream = std::move(candidate);
        resolvedPath = path;
        return true;
    }

    return false;
}
