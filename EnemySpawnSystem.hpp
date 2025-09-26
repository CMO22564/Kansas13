#pragma once

#include "Core.hpp"
#include "LevelData.hpp" // Required for EnemyTypeData
#include <SFML/System/Clock.hpp>

class EnemySpawnSystem : public System {
private:
    // Singleton boilerplate
    static EnemySpawnSystem* s_instance;

    // Internal state variables
    int m_maxEnemiesPerWave = 0; // Renamed to align with .cpp
    float m_spawnInterval = 0.0f;
    float m_minX = 0.0f;
    float m_maxX = 0.0f;
    sf::Clock spawnClock; // Using sf::Clock as per your .cpp implementation
    int m_enemiesSpawned = 0;
    int m_activeEnemyCount = 0;
    
    // Storage for the current enemy type data
    EnemyTypeData m_currentEnemyType; 

public:
    // Singleton access method
    static EnemySpawnSystem& getInstance();

    // 1. Corrected setLevelParameters to match all files
    void setLevelParameters(
        int count, 
        float interval, 
        float min_x, 
        float max_x,
        const EnemyTypeData& enemyTypeData
    );
    
    // 2. FIX: Add the full ECS update signature (Resolves main.cpp:114 error)
    void update(std::vector<EntityId>& entities,
                ComponentMap<PositionComponent>& positions,
                ComponentMap<VelocityComponent>& velocities,
                ComponentMap<RenderComponent>& shapes, // Must use RenderComponent
                ComponentMap<BouncingComponent>& bouncingShapes,
                ComponentMap<ActiveComponent>& activeStates,
                ComponentMap<DamageComponent>& damageValues);

    // 3. FIX: Add the missing function declaration (Resolves main.cpp:127 error)
    bool isLevelComplete() const; 
    
    void resetSpawner(); // Renamed to match the implementation in .cpp
};
