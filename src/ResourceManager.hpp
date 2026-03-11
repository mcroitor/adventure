#pragma once

#include <fstream>
#include <string>
#include <vector>

class ResourceManager {
public:
    static ResourceManager& Instance();

    bool FileExists(const std::string& path) const;
    bool OpenFirstFound(
        const std::vector<std::string>& paths,
        std::ifstream& stream,
        std::string& resolvedPath
    ) const;

private:
    ResourceManager() = default;
};
