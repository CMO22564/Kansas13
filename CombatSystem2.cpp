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
    ComponentMap<BouncingComponent>& bouncingShapes,
    ComponentMap<DamageComponent>& damages,
    ComponentMap<ActiveComponent>& actives,
    ComponentMap<PlayerHealthComponent>& playerHealths,
    ComponentMap<HealthComponent>& healths, // <-- Add this line
    ComponentMap<ShieldComponent>& shields,
    ComponentMap<SoundComponent>& sounds,
    ComponentMap<VelocityComponent>& velocities
) {
    GameStateManager& manager = GameStateManager::getInstance();
    
    // Find the player entity
    EntityId playerId = 0;
    for (auto& [id, health] : playerHealths) {
        playerId = id;
        break;
    }

    if (!playerId || !shapes.count(playerId) || !shapes.at(playerId).shape)
        return;
    
    // Using distance-based collision check for player-enemy
    for (auto& [enemyId, bouncingShape] : bouncingShapes) {
        if (!actives[enemyId].active) continue;
        if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;

        const auto& playerPos = positions.at(playerId).position;
        float playerRadius = shapes.at(playerId).size;
        const auto& enemyPos = positions.at(enemyId).position;
        float enemyRadius = shapes.at(enemyId).size;

        float dx = playerPos.x - enemyPos.x;
        float dy = playerPos.y - enemyPos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= (playerRadius + enemyRadius)) {
            // Apply damage to the player
            float damage = damages.count(enemyId) ? damages.at(enemyId).damage : 10.0f;
            
            if (shields[playerId].currentShield > 0) {
                shields[playerId].currentShield -= damage;
                if (shields[playerId].currentShield < 0) {
                    playerHealths.at(playerId).currentHealth += shields[playerId].currentShield;
                    shields[playerId].currentShield = 0;
                }
                sounds[enemyId].type = SoundComponent::Type::ShieldHit;
            } else {
                playerHealths.at(playerId).currentHealth -= damage;
                sounds[enemyId].type = SoundComponent::Type::PlayerHit;
            }

            actives[enemyId].active = false;
            std::cout << "Player hit! Health: " << playerHealths.at(playerId).currentHealth
                      << " Shield: " << shields[playerId].currentShield << std::endl;
        }
    }

    // Using distance-based collision check for projectiles-enemies
    for (auto& [projectileId, proj] : projectiles) {
        if (!actives[projectileId].active) continue;
        if (!shapes.count(projectileId) || !shapes.at(projectileId).shape) continue;
        if (!damages.count(projectileId)) continue; 

        const auto& projPos = positions.at(projectileId).position;
        float projRadius = shapes.at(projectileId).size;

        for (auto& [enemyId, bouncingShape] : bouncingShapes) {
            if (!actives[enemyId].active) continue;
            if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;
            if (!healths.count(enemyId)) continue; 

            const auto& enemyPos = positions.at(enemyId).position;
            float enemyRadius = shapes.at(enemyId).size;
            
            float dx = projPos.x - enemyPos.x;
            float dy = projPos.y - enemyPos.y;
            float distance = std::sqrt(dx * dx + dy * dy);

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
