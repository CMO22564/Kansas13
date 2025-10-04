// LevelLoader.cpp (Cleaned of extended characters)

#include "LevelLoader.hpp"
#include <fstream>
#include <iostream>
#include <json/json.hpp>

// for convenience
using json = nlohmann::json;

std::vector<LevelData> loadLevelsFromFile(const std::string& filename) {
    std::vector<LevelData> levels;
    std::ifstream ifs(filename);

    if (!ifs.is_open()) {
        std::cerr << "Error opening level file: " << filename << std::endl;
        return levels;
    }

    json j;
    ifs >> j; // <-- Error likely starts here (Line 19)

    // CRITICAL FIX: Iterate directly over the JSON array
    for (const auto& levelJson : j) {
        LevelData level;
        level.levelNumber = levelJson["levelNumber"].get<int>();
        level.difficulty = levelJson["difficulty"].get<std::string>();

        const auto& enemiesJson = levelJson["enemies"];
        level.enemyCount = enemiesJson["count"].get<int>();
        level.spawnInterval = enemiesJson["spawnInterval"].get<float>();
        level.minX = enemiesJson["spawnArea"]["minX"].get<float>();
        level.maxX = enemiesJson["spawnArea"]["maxX"].get<float>();

        // FIX: Ensure this line starts ONLY with 'for'
        for (const auto& typeJson : enemiesJson["types"]) { 
            EnemyTypeData typeData;
            typeData.type = typeJson["type"].get<std::string>();
            typeData.color = typeJson["color"].get<std::string>();
            typeData.size = typeJson["size"].get<float>();
            typeData.baseHealth = typeJson["baseHealth"].get<float>(); // Ensure this is present from the previous step!
            typeData.baseDamage = typeJson["baseDamage"].get<int>();
            typeData.baseSpeed = typeJson["baseSpeed"].get<float>();
            level.enemyTypes.push_back(typeData);
        }
        levels.push_back(level);
    } // End of main level iteration loop

    return levels;
}
