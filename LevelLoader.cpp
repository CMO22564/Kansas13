#include "LevelData.hpp"
#include "LevelLoader.hpp" // <-- Add the new header here
#include "json/json.hpp" // Path to your json.hpp file
#include <iostream>
#include <fstream>

// This function will be called automatically by the from_json function for LevelData.
void EnemyTypeData::from_json(const nlohmann::json& j) {
    type = j.at("type").get<std::string>();
    color = j.at("color").get<std::string>();
    size = j.at("size").get<float>();
    baseDamage = j.at("baseDamage").get<float>();
    baseSpeed = j.at("baseSpeed").get<float>();
}

// This function loads the entire level from a JSON object.
void LevelData::from_json(const nlohmann::json& j) {
    levelNumber = j.at("levelNumber").get<int>();
    difficulty = j.at("difficulty").get<std::string>();
    enemyCount = j.at("enemies").at("count").get<int>();
    spawnInterval = j.at("enemies").at("spawnInterval").get<float>();
    minX = j.at("enemies").at("spawnArea").at("minX").get<float>();
    maxX = j.at("enemies").at("spawnArea").at("maxX").get<float>();

    // Parse the array of enemy types.
    for (const auto& enemy_json : j.at("enemies").at("types")) {
        EnemyTypeData enemy_data;
        enemy_data.from_json(enemy_json); // Use the helper function
        enemyTypes.push_back(enemy_data);
    }
}

// The public function to load the levels from a file.
    std::vector<LevelData> loadLevelsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open level file at " << filePath << std::endl;
        return {};
    }

    nlohmann::json jsonData;
    file >> jsonData;

    std::vector<LevelData> levels;
    for (const auto& level_json : jsonData) {
        LevelData levelData;
        levelData.from_json(level_json);
        levels.push_back(levelData);
    }

    std::cout << "Successfully loaded " << levels.size() << " levels." << std::endl;
    return levels;
}
