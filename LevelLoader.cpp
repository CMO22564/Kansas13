#include "LevelLoader.hpp"
#include "LevelData.hpp" // Contains the updated LevelData and EnemyTypeData structs
// Include your JSON library header here, e.g.:
#include "json.hpp"

// Assuming LevelLoader.cpp includes the implementation for the loadLevelsFromFile declaration
std::vector<LevelData> loadLevelsFromFile(const std::string& filePath) {
    std::vector<LevelData> levels;

    // 1. Read and parse the JSON file contents into a json object (assuming 'j' is the root array)
    // ... (Your file reading logic here)
    // nlohmann::json j = ... 

    // Assuming 'j' is the root array of levels:
    for (const auto& levelJson : j) {
        LevelData level;
        
        // --- Read Top-Level LevelData ---
        level.levelNumber = levelJson["levelNumber"].get<int>();
        level.difficulty = levelJson["difficulty"].get<std::string>();

        // --- Navigate to the "enemies" object ---
        const auto& enemiesJson = levelJson["enemies"];
        level.enemyCount = enemiesJson["count"].get<int>();
        level.spawnInterval = enemiesJson["spawnInterval"].get<float>();
        level.minX = enemiesJson["spawnArea"]["minX"].get<float>();
        level.maxX = enemiesJson["spawnArea"]["maxX"].get<float>();
        
        // --- Navigate to and Read the "types" Array ---
        // This array holds the visual (type, color) and combat data.
        const auto& typesArray = enemiesJson["types"];

        for (const auto& typeJson : typesArray) {
            EnemyTypeData typeData;
            
            // Read the new visual and combat fields
            typeData.type = typeJson["type"].get<std::string>();     // "Triangle"
            typeData.color = typeJson["color"].get<std::string>();   // "Red"
            typeData.size = typeJson["size"].get<float>();
            typeData.baseDamage = typeJson["baseDamage"].get<int>();
            typeData.baseSpeed = typeJson["baseSpeed"].get<float>();
            
            level.enemyTypes.push_back(typeData);
        }

        levels.push_back(level);
    }

    return levels;
}
