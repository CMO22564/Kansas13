#pragma once
#include <string>
#include <vector>
#include "json/json.hpp" // Now include the full header here
// No need for the forward declaration of nlohmann::json

struct EnemyTypeData {
    std::string type;
    std::string color;
    float size;
    float baseDamage;
    float baseSpeed;

    void from_json(const nlohmann::json& j);
};

struct LevelData {
    int levelNumber;
    std::string difficulty;
    int enemyCount;
    float spawnInterval;
    float minX;
    float maxX;
    std::vector<EnemyTypeData> enemyTypes;

    void from_json(const nlohmann::json& j);
};
