#include "EnemySpawnSystem.hpp"
#include "Core.hpp" // For RenderComponent, PositionComponent, etc.
#include "LevelData.hpp" // For EnemyTypeData
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include "Utils.hpp"

EnemySpawnSystem* EnemySpawnSystem::s_instance = nullptr;

EnemySpawnSystem& EnemySpawnSystem::getInstance() {
    if (!s_instance) {
        s_instance = new EnemySpawnSystem();
    }
    return *s_instance;
}

// 1. CRITICAL FIX: Implement the new setLevelParameters signature
void EnemySpawnSystem::setLevelParameters(int count, float interval, float min_x, float max_x,
                                         const EnemyTypeData& enemyTypeData) {
    m_maxEnemiesPerWave = count;
    m_spawnInterval = interval;
    m_minX = min_x;
    m_maxX = max_x;
    
    // Store the enemy type data for use in spawning
    m_currentEnemyType = enemyTypeData; 
    
    resetSpawner(); // Reset all spawning variables
}

void EnemySpawnSystem::resetSpawner() {
    m_enemiesSpawned = 0;
    m_activeEnemyCount = 0;
    spawnClock.restart();
    std::cout << "Enemy Spawner reset for new level. Max Enemies: " 
              << m_maxEnemiesPerWave << ", Type: " 
              << m_currentEnemyType.type << ", Color: " 
              << m_currentEnemyType.color << std::endl;
}

bool EnemySpawnSystem::isLevelComplete() const {
    // Level is complete ONLY IF all enemies have been spawned AND all are destroyed.
    return (m_enemiesSpawned >= m_maxEnemiesPerWave) && (m_activeEnemyCount == 0);
}


void EnemySpawnSystem::update(std::vector<EntityId>& entities,
                              ComponentMap<PositionComponent>& positions,
                              ComponentMap<VelocityComponent>& velocities,
                              ComponentMap<RenderComponent>& shapes, // Use RenderComponent
                              ComponentMap<BouncingComponent>& bouncingShapes,
                              ComponentMap<ActiveComponent>& activeStates,
                              ComponentMap<DamageComponent>& damages, // Changed from damageValues
                              ComponentMap<HealthComponent>& healths,
                              ComponentMap<EnemyComponent>& enemies) { // <-- NOTE: EnemyComponent map is now passed in

    // First, handle the spawning of enemies
    if (m_enemiesSpawned < m_maxEnemiesPerWave) {
        if (spawnClock.getElapsedTime().asSeconds() >= m_spawnInterval) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distX(m_minX, m_maxX);
            float spawnX = distX(gen);

            EntityId enemyId = getNextEntityId();
            entities.push_back(enemyId);
            
            // Set Position Component
            positions.emplace(enemyId, PositionComponent{ sf::Vector2f(spawnX, 50.f) });
            
            // Use baseSpeed from EnemyTypeData
            velocities.emplace(enemyId, VelocityComponent{ sf::Vector2f(0.0f, m_currentEnemyType.baseSpeed) });
            
            // Use the new EnemyTypeData for the RenderComponent
            RenderComponent renderData;
            
            // Convert string fields to enum and sf::Color
            renderData.type = shapeTypeFromString(m_currentEnemyType.type);
            renderData.color = colorFromString(m_currentEnemyType.color);
            // Use the single float size
            renderData.size = m_currentEnemyType.size;
            
            shapes.emplace(enemyId, std::move(renderData));
            
            // Set other components
            activeStates.emplace(enemyId, ActiveComponent{ true });
            bouncingShapes.emplace(enemyId, BouncingComponent{});
            
            // CRITICAL ADDITION: Add the EnemyComponent tag
            enemies.emplace(enemyId, EnemyComponent{});
            
            // ✅ REQUIRED FIX: Use the confirmed 'baseHealth' member
            healths.emplace(enemyId, HealthComponent{m_currentEnemyType.baseHealth, m_currentEnemyType.baseHealth}); 
            
            // ✅ REQUIRED FIX: Use the confirmed 'baseDamage' member and static_cast
            damages.emplace(enemyId, DamageComponent{ static_cast<float>(m_currentEnemyType.baseDamage) }); // Changed from damageValues
            
            m_enemiesSpawned++;
            spawnClock.restart();
            std::cout << "Enemy spawned at (" << spawnX << ", 50) - Total: " << m_enemiesSpawned << "/" << m_maxEnemiesPerWave << std::endl;
        }
    }
    
    // Update the count of active enemies
    m_activeEnemyCount = 0;
    // FIX: Iterate over the EnemyComponent map to accurately count active enemies
    for (auto& [e, enemyTag] : enemies) {
        // Check if the entity is active and has the required components
        if (activeStates.count(e) && activeStates.at(e).active) {
            m_activeEnemyCount++;
        }
    }
    
    if (isLevelComplete()) {
        std::cout << "All enemies destroyed - Level completion signaled!" << std::endl;
    }
}
