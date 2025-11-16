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
    ComponentMap<EnemyComponent>& enemies, // <-- CRITICAL: ADDED ComponentMap for the Enemy Tag
    ComponentMap<PlayerLivesComponent>& playerLives
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
    
        
// CombatSystem.cpp (Around line 30)

// Check for Shield Warning
if (shields.count(playerId)) {
    auto& shield = shields.at(playerId);
    float shieldRatio = (shield.maxShield > 0) ? shield.currentShield / shield.maxShield : 0.0f;
    
    // --- 1. Trigger Warning (Plays sound each time shield drops below 25%) ---
    if (shieldRatio <= 0.25f && shieldRatio > 0.0f) { 
       if (!shield.isWarningActive) {
            // Trigger the sound ONLY ONCE when crossing the threshold
            if (!sounds.count(playerId)) sounds.emplace(playerId, SoundComponent{});
            sounds.at(playerId).type = SoundComponent::Type::ShieldWarning;
            
            // Activate the flag to stop spamming until the shield recovers
            shield.isWarningActive = true; 
        } 
    } 
    // --- 2. Reset Flag if Shield Recovers (Allows sound to re-trigger later) ---
    else if (shieldRatio > 0.25f) {
        // Shield has recovered above the warning threshold, reset the flag.
        shield.isWarningActive = false;
    }
}

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
    // In CombatSystem::update, replace the Game Over block
    if (playerId) {
        auto healthIt = playerHealths.find(playerId);
        auto livesIt = playerLives.find(playerId);
        auto posIt = positions.find(playerId);

        if (healthIt != playerHealths.end() && livesIt != playerLives.end() && posIt != positions.end()) {
            // Check for player death
            if (healthIt->second.currentHealth <= 0.0f) {
                // Decrement lives
                livesIt->second.lives--;
                std::cout << "Player died! Lives remaining: " << livesIt->second.lives << std::endl;

                // Check for Game Over or Score Entry
                if (livesIt->second.lives <= 0) {
                    GameStateManager& manager = GameStateManager::getInstance();
                    // Check if score qualifies for high score list
                    if (manager.getHighScoreManager().isHighScore(manager.getScore())) {
                        manager.setState(GameState::ScoreEntry); // Transition to score entry
                    } else {
                        manager.setState(GameState::GameOver); // Go directly to Game Over
                    }
                    
                    if (!sounds.count(playerId)) sounds.emplace(playerId, SoundComponent{});
                    sounds.at(playerId).type = SoundComponent::Type::GameOver;
                    
                    std::cout << "GAME OVER" << std::endl;
               
                    // CombatSystem.cpp (Find this block near the end of the update function)

                // CombatSystem.cpp (Inside the player respawn 'else' block)

                } else {
                    // Respawn Player: Reset Health and Position
                    healthIt->second.currentHealth = healthIt->second.maxHealth;
                    posIt->second.position = sf::Vector2f(400.0f, 300.0f);
                    
                    // Reset Shield
                    auto shieldIt = shields.find(playerId);
                    if (shieldIt != shields.end()) {
                        shieldIt->second.currentShield = shieldIt->second.maxShield;
                        // 🛑 FIX: Reset the warning flag when shields are restored to max (New Life)
                        shieldIt->second.isWarningActive = false; 
                    }
                    
                    // Play respawn sound
                    if (!sounds.count(playerId)) sounds.emplace(playerId, SoundComponent{});
                    sounds.at(playerId).type = SoundComponent::Type::Respawn;
                }
            }
        }
    }
}
