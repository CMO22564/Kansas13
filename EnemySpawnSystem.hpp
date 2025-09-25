#pragma once
#include "Core.hpp"
#include <SFML/System/Clock.hpp>

class EnemySpawnSystem : public System {
public:
    static EnemySpawnSystem& getInstance(); // Assuming singleton pattern like GameStateManager

    // New methods for level control
    void setLevelParameters(int count, float interval, float min_x, float max_x);
    bool isLevelComplete() const;
    void resetSpawner();

    // Existing update method
    void update(std::vector<EntityId>& entities,
                ComponentMap<PositionComponent>& positions,
                ComponentMap<VelocityComponent>& velocities,
                ComponentMap<ShapeComponent>& shapes,
                ComponentMap<BouncingComponent>& bouncingShapes,
                ComponentMap<ActiveComponent>& activeStates,
                ComponentMap<DamageComponent>& damageValues);                
private:
    static EnemySpawnSystem* s_instance; // For singleton
    sf::Clock spawnClock;
    
    // Variables to store level data
    int m_enemiesSpawned = 0;
    int m_maxEnemiesPerWave = 0;
    float m_spawnInterval = 0.0f;
    float m_minX = 0.0f;
    float m_maxX = 0.0f;
    int m_activeEnemyCount = 0; // Tracks currently alive enemies
};
