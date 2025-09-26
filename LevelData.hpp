// LevelData.hpp
#pragma once

#include <vector>
#include <string>

// Include necessary header for SFML Color if you're using it globally
// For now, we'll keep the color as a string and convert it later.

// 1. New struct to hold the visual and combat details for a specific enemy type
struct EnemyTypeData {
    std::string type;       // e.g., "Circle", "Triangle" (for the RenderSystem)
    std::string color;      // e.g., "Blue", "Red" 
    float size;             // e.g., 20, 25 (for the RenderSystem)
    int baseDamage;
    float baseSpeed;
};

// 2. Updated main LevelData struct
struct LevelData {
    int levelNumber;
    std::string difficulty;
    
    // Core spawning parameters
    int enemyCount;
    float spawnInterval;
    float minX, maxX;
    
    // Storage for the enemy type(s) defined in the JSON's "types" array
    // This connects to the "types": [ ... ] array in your JSON file.
    std::vector<EnemyTypeData> enemyTypes; 
};
