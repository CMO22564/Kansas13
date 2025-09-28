#include <cmath>
#include "Core.hpp"
#include "CombatSystem.hpp"
#include "GameStateManager.hpp"
#include <iostream>

void CombatSystem::update(
        std::vector<EntityId>& entities,
        ComponentMap<PositionComponent>& positions,
        ComponentMap<RenderComponent>& shapes,
        ComponentMap<ProjectileComponent>& projectiles,
        ComponentMap<BouncingComponent>& bouncings,
        ComponentMap<DamageComponent>& damages,
        ComponentMap<ActiveComponent>& actives,
        ComponentMap<PlayerHealthComponent>& playerHealths,
        ComponentMap<HealthComponent>& healths, // This is the line you need to add back
        ComponentMap<ShieldComponent>& shields,
        ComponentMap<SoundComponent>& sounds,
        ComponentMap<VelocityComponent>& velocities
        /* int& score removed */)
{
        
    GameStateManager& manager = GameStateManager::getInstance();
    
        
        // Find the player entity - unchanged.
        EntityId playerId = 0;
        for (auto& [id, health] : playerHealths) {
            playerId = id;
            break;
        }

        if (!playerId || !shapes.count(playerId) || !shapes.at(playerId).shape)
            return;

        sf::FloatRect playerBounds = shapes.at(playerId).shape->getGlobalBounds();
        auto& playerHealth = playerHealths.at(playerId);
        auto& playerShield = shields[playerId]; // default constructed if missing

        // --- Enemy collisions with player ---
for (auto& [enemyId, bouncing] : bouncings) {
    if (!actives[enemyId].active) continue;
    if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;

    // Get the player's position and size
    const auto& playerPos = positions.at(playerId).position;
    float playerRadius = shapes.at(playerId).size;

    // Get the enemy's position and size
    const auto& enemyPos = positions.at(enemyId).position;
    float enemyRadius = shapes.at(enemyId).size;

    // Calculate the distance between the two circles' centers
    float dx = playerPos.x - enemyPos.x;
    float dy = playerPos.y - enemyPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Check for collision using distance vs. sum of radii
    if (distance <= (playerRadius + enemyRadius)) {
        float damage = damages.count(enemyId) ? damages.at(enemyId).damage : 10.0f;

        if (playerShield.currentShield > 0) {
            playerShield.currentShield -= damage;
            if (playerShield.currentShield < 0) {
                playerHealth.currentHealth += playerShield.currentShield;
                playerShield.currentShield = 0;
            }
            sounds[enemyId].type = SoundComponent::Type::ShieldHit;
        } else {
            playerHealth.currentHealth -= damage;
            sounds[enemyId].type = SoundComponent::Type::PlayerHit;
        }

        actives[enemyId].active = false;
        std::cout << "Player hit! Health: " << playerHealth.currentHealth
                  << " Shield: " << playerShield.currentShield << std::endl;
    }
}

// --- Projectile collisions with enemies ---
for (auto& [projectileId, proj] : projectiles) {
    if (!actives[projectileId].active) continue;
    if (!shapes.count(projectileId) || !shapes.at(projectileId).shape) continue;
    if (!damages.count(projectileId)) continue; 

    // Get the projectile's position and size
    const auto& projPos = positions.at(projectileId).position;
    float projRadius = shapes.at(projectileId).size;

    for (auto& [enemyId, bouncing] : bouncings) {
        if (!actives[enemyId].active) continue;
        if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;
        if (!healths.count(enemyId)) continue; 

        // Get the enemy's position and size
        const auto& enemyPos = positions.at(enemyId).position;
        float enemyRadius = shapes.at(enemyId).size;
        
        // Calculate the distance between the two circles' centers
        float dx = projPos.x - enemyPos.x;
        float dy = projPos.y - enemyPos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Check for collision using distance vs. sum of radii
        if (distance <= (projRadius + enemyRadius)) {
            // Apply damage to the enemy
            float projectileDamage = damages.at(projectileId).damage;
            healths.at(enemyId).currentHealth -= projectileDamage;

            // Mark the projectile for destruction
            actives[projectileId].active = false;

            std::cout << "Enemy " << enemyId << " hit. Health: " << healths.at(enemyId).currentHealth << std::endl;

            // Check if the enemy is destroyed
            if (healths.at(enemyId).currentHealth <= 0) {
                actives[enemyId].active = false;
                GameStateManager::getInstance().setScore(GameStateManager::getInstance().getScore() + 100);
                sounds[projectileId].type = SoundComponent::Type::Explosion;
                std::cout << "Enemy destroyed! Score: " << GameStateManager::getInstance().getScore() << std::endl;
            }

            break; 
        }
    }	
  }
}
