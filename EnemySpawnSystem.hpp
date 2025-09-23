#pragma once
#include <vector>
#include "Core.hpp"
#include <SFML/System.hpp>

class EnemySpawnSystem {
private:
    sf::Clock spawnClock;
    float spawnInterval = 2.0f; // Spawn every 2 seconds
    float minX = 50.f; // Spawn within window bounds
    float maxX = 750.f;
    int enemiesSpawned = 0;
    int maxEnemiesPerWave = 25; // Adjust for difficulty
public:
    void update(std::vector<EntityId>& entities,
                ComponentMap<PositionComponent>& positions,
                ComponentMap<VelocityComponent>& velocities,
                ComponentMap<ShapeComponent>& shapes,
                ComponentMap<BouncingComponent>& bouncingShapes,
                ComponentMap<ActiveComponent>& activeStates,
                ComponentMap<DamageComponent>& damageValues);
};
