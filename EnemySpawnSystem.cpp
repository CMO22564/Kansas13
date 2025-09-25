#include "EnemySpawnSystem.hpp"
#include "Core.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

EnemySpawnSystem* EnemySpawnSystem::s_instance = nullptr;

EnemySpawnSystem& EnemySpawnSystem::getInstance() {
    if (!s_instance) {
        s_instance = new EnemySpawnSystem();
    }
    return *s_instance;
}

void EnemySpawnSystem::setLevelParameters(int count, float interval, float min_x, float max_x) {
    m_maxEnemiesPerWave = count;
    m_spawnInterval = interval;
    m_minX = min_x;
    m_maxX = max_x;
    resetSpawner(); // Reset all spawning variables
}

void EnemySpawnSystem::resetSpawner() {
    m_enemiesSpawned = 0;
    m_activeEnemyCount = 0;
    spawnClock.restart();
    std::cout << "Enemy Spawner reset for new level. Max Enemies: " << m_maxEnemiesPerWave << std::endl;
}

bool EnemySpawnSystem::isLevelComplete() const {
    // Level is complete ONLY IF all enemies have been spawned AND all are destroyed.
    return (m_enemiesSpawned >= m_maxEnemiesPerWave) && (m_activeEnemyCount == 0);
}


void EnemySpawnSystem::update(std::vector<EntityId>& entities,
                              ComponentMap<PositionComponent>& positions,
                              ComponentMap<VelocityComponent>& velocities,
                              ComponentMap<ShapeComponent>& shapes,
                              ComponentMap<BouncingComponent>& bouncingShapes,
                              ComponentMap<ActiveComponent>& activeStates,
                              ComponentMap<DamageComponent>& damageValues) {

    // First, handle the spawning of enemies
    if (m_enemiesSpawned < m_maxEnemiesPerWave) {
        if (spawnClock.getElapsedTime().asSeconds() >= m_spawnInterval) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distX(m_minX, m_maxX);
            float spawnX = distX(gen);

            EntityId enemyId = getNextEntityId();
            entities.push_back(enemyId);
            
            // ... (Spawning component setup using m_enemiesSpawned for variation) ...
            positions.emplace(enemyId, PositionComponent{ sf::Vector2f(spawnX, 50.f) });
            velocities.emplace(enemyId, VelocityComponent{ sf::Vector2f(100.f + (m_enemiesSpawned * 10.f), 20.f) });
            
            ShapeComponent enemyShape;
            enemyShape.type = ShapeComponent::Type::Circle; 
            enemyShape.color = sf::Color::Blue;
            enemyShape.size = sf::Vector2f(20.f, 20.f);
            
            shapes.emplace(enemyId, std::move(enemyShape));
            
            activeStates.emplace(enemyId, ActiveComponent{ true });
            bouncingShapes.emplace(enemyId, BouncingComponent{});
            damageValues.emplace(enemyId, DamageComponent{ 10.f + (m_enemiesSpawned * 2.f) }); 
            
            m_enemiesSpawned++; 
            spawnClock.restart();
            std::cout << "Enemy spawned at (" << spawnX << ", 50) - Total: " << m_enemiesSpawned << "/" << m_maxEnemiesPerWave << std::endl;
        }
    }
    
    // Update the count of active enemies
    m_activeEnemyCount = 0;
    for (EntityId e : entities) {
        if (bouncingShapes.count(e) && activeStates.count(e) && activeStates[e].active) {
            m_activeEnemyCount++;
        }
    }
    
    if (isLevelComplete()) {
        // Log that the signal is being sent, but GameStateManager handles the advance
        // This log helps debug where the transition is initiated
        std::cout << "All enemies destroyed - Level completion signaled!" << std::endl;
    }
}
