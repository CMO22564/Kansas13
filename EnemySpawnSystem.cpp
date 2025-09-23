#include "EnemySpawnSystem.hpp"
#include "Core.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

void EnemySpawnSystem::update(std::vector<EntityId>& entities,
                              ComponentMap<PositionComponent>& positions,
                              ComponentMap<VelocityComponent>& velocities,
                              ComponentMap<ShapeComponent>& shapes,
                              ComponentMap<BouncingComponent>& bouncingShapes,
                              ComponentMap<ActiveComponent>& activeStates,
                              ComponentMap<DamageComponent>& damageValues) {

    // First, handle the spawning of enemies
    if (enemiesSpawned < maxEnemiesPerWave) {
        // Spawn enemies every spawnInterval seconds
        if (spawnClock.getElapsedTime().asSeconds() >= spawnInterval) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distX(minX, maxX);
            float spawnX = distX(gen);

            EntityId enemyId = getNextEntityId();
            entities.push_back(enemyId);
            
            // Position: Random X, fixed Y (top of screen)
            positions.emplace(enemyId, PositionComponent{ sf::Vector2f(spawnX, 50.f) });
            
            // Velocity: Move right/down (adjust for difficulty)
            velocities.emplace(enemyId, VelocityComponent{ sf::Vector2f(100.f + (enemiesSpawned * 10.f), 20.f) });
            
            // ⭐ FIX: Correctly initialize ShapeComponent with data, but don't create the SFML shape here.
            ShapeComponent enemyShape;
            enemyShape.type = ShapeComponent::Type::Circle; // You can change this to Triangle later if you want
            enemyShape.color = sf::Color::Blue;
            enemyShape.size = sf::Vector2f(20.f, 20.f);
            
            // ⭐ FIX: Use std::move to transfer ownership of the ShapeComponent
            shapes.emplace(enemyId, std::move(enemyShape));
            
            activeStates.emplace(enemyId, ActiveComponent{ true });
            bouncingShapes.emplace(enemyId, BouncingComponent{});
            damageValues.emplace(enemyId, DamageComponent{ 10.f + (enemiesSpawned * 2.f) }); // Increasing damage
            
            enemiesSpawned++;
            spawnClock.restart();
            std::cout << "Enemy spawned at (" << spawnX << ", 50) - Total: " << enemiesSpawned << "/" << maxEnemiesPerWave << std::endl;
        }
    }
    
    // Check if the current wave is clear and reset for the next one
    int activeEnemies = 0;
    for (EntityId e : entities) {
        if (bouncingShapes.count(e) && activeStates.count(e) && activeStates[e].active) {
            activeEnemies++;
        }
    }
    
    // If all enemies are destroyed, reset the spawner for the next wave
    if (activeEnemies == 0 && enemiesSpawned >= maxEnemiesPerWave) {
        enemiesSpawned = 0;
        spawnClock.restart();
        std::cout << "All enemies destroyed - Ready for next wave!" << std::endl;
    }
}
