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
    ComponentMap<DamageComponent>& damageValues,
    ComponentMap<ActiveComponent>& actives,
    ComponentMap<PlayerHealthComponent>& playerHealths,
    ComponentMap<ShieldComponent>& shields,
    ComponentMap<SoundComponent>& sounds,
    ComponentMap<VelocityComponent>& velocities) {

    GameStateManager& manager = GameStateManager::getInstance();

    // Find the player entity
    EntityId playerId = 0;
    for (auto& [id, health] : playerHealths) {
        playerId = id;
        break;
    }

    if (!playerId || !shapes.count(playerId) || !shapes.at(playerId).shape)
        return;

    sf::FloatRect playerBounds = shapes.at(playerId).shape->getGlobalBounds();
    auto& playerHealth = playerHealths.at(playerId);
    auto& playerShield = shields[playerId]; // Default constructed if missing

    // --- Enemy collisions with player ---
    for (auto& [enemyId, bouncingShape] : bouncingShapes) {
        if (!actives[enemyId].active) continue;
        if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;

        sf::FloatRect enemyBounds = shapes.at(enemyId).shape->getGlobalBounds();

        // Use intersects() with output parameter (SFML 3.x style)
        sf::FloatRect intersection;
        if (playerBounds.intersects(enemyBounds, intersection)) {
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
        if (!damageValues.count(projectileId)) continue;

        sf::FloatRect projectileBounds = shapes.at(projectileId).shape->getGlobalBounds();

        for (auto& [enemyId, bouncingShape] : bouncingShapes) {
            if (!actives[enemyId].active) continue;
            if (!shapes.count(enemyId) || !shapes.at(enemyId).shape) continue;

            sf::FloatRect enemyBounds = shapes.at(enemyId).shape->getGlobalBounds();
            sf::FloatRect intersection;

            if (projectileBounds.intersects(enemyBounds, intersection)) {
                // Deactivate projectile
                actives[projectileId].active = false;

                // Apply damage to enemy (assuming HealthComponent exists)
                if (healths.count(enemyId)) {
                    float projectileDamage = damageValues.at(projectileId).damage;
                    auto& enemyHealth = healths.at(enemyId);
                    enemyHealth.currentHealth -= projectileDamage;

                    std::cout << "Enemy " << enemyId << " hit. Health: " << enemyHealth.currentHealth << std::endl;

                    // Deactivate enemy if health <= 0
                    if (enemyHealth.currentHealth <= 0) {
                        actives[enemyId].active = false;
                        GameStateManager::getInstance().setScore(GameStateManager::getInstance().getScore() + 100);
                        sounds[enemyId].type = SoundComponent::Type::Explosion;
                        std::cout << "Enemy destroyed! Score: " << GameStateManager::getInstance().getScore() << std::endl;
                    }
                } else {
                    // Fallback: Deactivate enemy directly if no health component
                    actives[enemyId].active = false;
                    GameStateManager::getInstance().setScore(GameStateManager::getInstance().getScore() + 100);
                    sounds[enemyId].type = SoundComponent::Type::Explosion;
                    std::cout << "Enemy destroyed! Score: " << GameStateManager::getInstance().getScore() << std::endl;
                }

                break; // Exit inner loop after hit
            }
        }
    }
}
