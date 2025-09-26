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

            sf::FloatRect enemyBounds = shapes.at(enemyId).shape->getGlobalBounds();
            sf::FloatRect intersection;

            if (playerBounds.findIntersection(enemyBounds).has_value()) {
                float damage = damages.count(enemyId) ? damages.at(enemyId).damage : 10.0f;

                if (playerShield.currentShield > 0) {
                    playerShield.currentShield -= damage;
                    if (playerShield.currentShield < 0) {
                        playerHealth.currentHealth += playerShield.currentShield; // overflow damage
                        playerShield.currentShield = 0;
                    }
                    sounds[enemyId].type = SoundComponent::Type::ShieldHit;
                } else {
                    playerHealth.currentHealth -= damage;
                    sounds[enemyId].type = SoundComponent::Type::PlayerHit;
                }

                actives[enemyId].active = false; // enemy destroyed
                std::cout << "Player hit! Health: " << playerHealth.currentHealth
                          << " Shield: " << playerShield.currentShield << std::endl;
            }
        }

        // --- Projectile collisions with enemies ---
    for (auto& [projectileId, proj] : projectiles) {
        if (!actives[projectileId].active) continue;
        if (!shapes.count(projectileId) || !shapes.at(projectileId).shape) continue;

        sf::FloatRect projectileBounds = shapes.at(projectileId).shape->getGlobalBounds();

        for (auto& [enemyId, bouncing] : bouncings) {
            if (!actives[enemyId].active) continue;
            if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;

            sf::FloatRect enemyBounds = shapes.at(enemyId).shape->getGlobalBounds();

            if (projectileBounds.findIntersection(enemyBounds).has_value()) {
                // float damage = damages.count(projectileId) ? damages.at(projectileId).damage : 10.0f; // Removed unused variable
                actives[enemyId].active = false;      // enemy destroyed
                actives[projectileId].active = false; // projectile consumed
                
                // CRITICAL FIX: Update the score via the GameStateManager
                int points = 100;
                manager.setScore(manager.getScore() + points);
                
                sounds[projectileId].type = SoundComponent::Type::Explosion;

                std::cout << "Enemy destroyed! Score: " << manager.getScore() << std::endl;
            }
        }
    }
}
