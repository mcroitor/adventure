/**
 * @file test_resource_manager.cpp
 * @brief Unit tests for ResourceManager
 */

#include <catch2/catch_test_macros.hpp>

#include "ResourceManager.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

std::filesystem::path MakeUniquePath(const std::string& prefix) {
    const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() / (prefix + std::to_string(tick) + ".txt");
}

} // namespace

TEST_CASE("ResourceManager detects existing files", "[ResourceManager]") {
    const auto path = MakeUniquePath("adventure_resource_exists_");
    {
        std::ofstream out(path.string());
        out << "ok";
    }

    REQUIRE(ResourceManager::Instance().FileExists(path.string()));

    std::filesystem::remove(path);
    REQUIRE_FALSE(ResourceManager::Instance().FileExists(path.string()));
}

TEST_CASE("ResourceManager opens first existing path", "[ResourceManager]") {
    const auto missingFirst = MakeUniquePath("adventure_resource_missing_first_");
    const auto existing = MakeUniquePath("adventure_resource_existing_");
    const auto missingLast = MakeUniquePath("adventure_resource_missing_last_");

    {
        std::ofstream out(existing.string());
        out << "quest data";
    }

    std::ifstream input;
    std::string resolved;
    const std::vector<std::string> paths = {
        missingFirst.string(),
        existing.string(),
        missingLast.string()
    };

    REQUIRE(ResourceManager::Instance().OpenFirstFound(paths, input, resolved));
    REQUIRE(input.is_open());
    REQUIRE(resolved == existing.string());

    std::string line;
    std::getline(input, line);
    REQUIRE(line == "quest data");

    input.close();
    std::filesystem::remove(existing);
}

TEST_CASE("ResourceManager reports failure when no file exists", "[ResourceManager]") {
    std::ifstream input;
    std::string resolved = "unchanged";
    const std::vector<std::string> paths = {
        MakeUniquePath("adventure_resource_missing_a_").string(),
        MakeUniquePath("adventure_resource_missing_b_").string()
    };

    REQUIRE_FALSE(ResourceManager::Instance().OpenFirstFound(paths, input, resolved));
    REQUIRE_FALSE(input.is_open());
    REQUIRE(resolved == "unchanged");
}
