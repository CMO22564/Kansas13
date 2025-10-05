#include <cmath>
#include <iostream>
#include "Core.hpp"
#include "CombatSystem.hpp"
#include "GameStateManager.hpp"
// Include the EnemyComponent header if it's in a separate file, 
// but it's currently defined in Core.hpp, so we're good.

void CombatSystem::update(
    std::vector<EntityId>& entities,
    ComponentMap<PositionComponent>& positions,
    ComponentMap<RenderComponent>& shapes,
    ComponentMap<ProjectileComponent>& projectiles,
    ComponentMap<BouncingComponent>& bouncingShapes, // NOTE: Still passed, but now optional for collision loops
    ComponentMap<DamageComponent>& damages, // Keep only this one. Removed ComponentMap<DamageComponent>& damageValues,
    ComponentMap<ActiveComponent>& actives,
    ComponentMap<PlayerHealthComponent>& playerHealths,
    ComponentMap<HealthComponent>& healths,
    ComponentMap<ShieldComponent>& shields,
    ComponentMap<SoundComponent>& sounds,
    ComponentMap<VelocityComponent>& velocities,
    ComponentMap<EnemyComponent>& enemies // <-- CRITICAL: ADDED ComponentMap for the Enemy Tag
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
    
    // --- 1. Player-Enemy Collision Check ---
    // Change: Iterate over the new EnemyComponent map instead of BouncingComponent
    for (auto& [enemyId, enemyTag] : enemies) { 
        if (!actives.count(enemyId) || !actives.at(enemyId).active) continue;
        if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;

        // Ensure components exist before accessing them via .at()
        if (!positions.count(playerId) || !positions.count(enemyId) || 
            !shapes.count(playerId) || !shapes.count(enemyId) ||
            !playerHealths.count(playerId) || !shields.count(playerId)) {
            continue;
        }

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
            
            // Shield logic
            if (shields.at(playerId).currentShield > 0) {
                shields.at(playerId).currentShield -= damage;
                
                if (shields.at(playerId).currentShield < 0) {
                    playerHealths.at(playerId).currentHealth += shields.at(playerId).currentShield;
                    shields.at(playerId).currentShield = 0;
                }
                if (!sounds.count(enemyId)) sounds.emplace(enemyId, SoundComponent{});
                sounds.at(enemyId).type = SoundComponent::Type::ShieldHit;
            } else {
                playerHealths.at(playerId).currentHealth -= damage;
                if (!sounds.count(enemyId)) sounds.emplace(enemyId, SoundComponent{});
                sounds.at(enemyId).type = SoundComponent::Type::PlayerHit;
            }

            actives.at(enemyId).active = false;
            std::cout << "Player hit! Health: " << playerHealths.at(playerId).currentHealth
                      << " Shield: " << shields.at(playerId).currentShield << std::endl;
        }
    }

    // --- 2. Projectile-Enemy Collision Check ---
    for (auto& [projectileId, proj] : projectiles) {
        // Basic projectile checks
        if (!actives.count(projectileId) || !actives.at(projectileId).active) continue;
        if (!shapes.count(projectileId) || !shapes.at(projectileId).shape) continue;
        if (!damages.count(projectileId)) continue; 
        if (!positions.count(projectileId)) continue; 

        const auto& projPos = positions.at(projectileId).position;
        float projRadius = shapes.at(projectileId).size;
        float projectileDamage = damages.at(projectileId).damage; 

        // Change: Iterate over the new EnemyComponent map instead of BouncingComponent
        for (auto& [enemyId, enemyTag] : enemies) {
            // Basic enemy checks
            if (!actives.count(enemyId) || !actives.at(enemyId).active) continue;
            if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;
            if (!healths.count(enemyId)) continue;
            if (!positions.count(enemyId)) continue; 

            const auto& enemyPos = positions.at(enemyId).position;
            float enemyRadius = shapes.at(enemyId).size;
            
            float dx = projPos.x - enemyPos.x;
            float dy = projPos.y - enemyPos.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= (projRadius + enemyRadius)) {
                // Apply damage
                healths.at(enemyId).currentHealth -= projectileDamage;

                // Mark the projectile for destruction
                actives.at(projectileId).active = false;
                
                std::cout << "Enemy " << enemyId << " hit. Health: " << healths.at(enemyId).currentHealth << std::endl;

                // Check if the enemy is destroyed
                if (healths.at(enemyId).currentHealth <= 0) {
                    actives.at(enemyId).active = false;
                    GameStateManager::getInstance().setScore(GameStateManager::getInstance().getScore() + 100);
                    
                    if (!sounds.count(projectileId)) sounds.emplace(projectileId, SoundComponent{});
                    sounds.at(projectileId).type = SoundComponent::Type::Explosion;
                    
                    std::cout << "Enemy destroyed! Score: " << GameStateManager::getInstance().getScore() << std::endl;
                }
                
                break; // Stop checking this projectile against other enemies
            }
        }
    }
}
