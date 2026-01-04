#include "CombatSystem.hpp"
#include "GameStateManager.hpp"
#include <cmath>
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
    ComponentMap<HealthComponent>& healths,
    ComponentMap<ShieldComponent>& shields,
    ComponentMap<SoundComponent>& sounds,
    ComponentMap<VelocityComponent>& velocities,
    ComponentMap<EnemyComponent>& enemies,
    ComponentMap<PlayerLivesComponent>& playerLives,
    ComponentMap<PlayerInputComponent>& playerInputs) {

    GameStateManager& manager = GameStateManager::getInstance();
    if (manager.getState() != GameState::Running) return;

    // 1. Find Player
    EntityId playerId = 0;
    for (auto& [id, health] : playerHealths) {
        playerId = id;
        break;
    }
    if (playerId == 0) return;

    // --- 2. Collision: Projectiles vs Enemies ---
    for (auto projId : entities) {
        if (!projectiles.count(projId) || !actives.count(projId) || !actives.at(projId).active) continue;

        for (auto& [enemyId, enemy] : enemies) {
            if (!actives.count(enemyId) || !actives.at(enemyId).active) continue;

            float distSq = std::pow(positions.at(projId).position.x - positions.at(enemyId).position.x, 2) +
                           std::pow(positions.at(projId).position.y - positions.at(enemyId).position.y, 2);
            float radiusSum = (shapes.at(projId).size + shapes.at(enemyId).size);

            if (distSq < std::pow(radiusSum, 2)) {
                healths.at(enemyId).currentHealth -= damages.at(projId).damage;
                actives.at(projId).active = false;

                if (healths.at(enemyId).currentHealth <= 0) {
                    actives.at(enemyId).active = false;

                    // 🌟 FIX: Use getScore/setScore instead of addScore
                    int currentScore = manager.getScore();
                    manager.setScore(currentScore + (enemy.generation * 100));

                    // Add Credits
                    if (playerInputs.count(playerId)) {
                        playerInputs.at(playerId).credits += enemy.generation * 15;
                    }

                    // Handle Splitting
                    if (enemy.generation > 1) {
                        for (int i = 0; i < 2; ++i) {
                            EntityId childId = getNextEntityId();
                            entities.push_back(childId);
                            
                            positions[childId] = positions.at(enemyId);
                            actives[childId] = { true };
                            
                            // Safe Manual Copy of RenderComponent
                            RenderComponent& parentRender = shapes.at(enemyId);
                            RenderComponent childRender;
                            childRender.type = parentRender.type;
                            childRender.color = parentRender.color;
                            childRender.size = parentRender.size * 0.7f;
                            shapes[childId] = std::move(childRender);

                            EnemyComponent childEnemy = enemy;
                            childEnemy.generation--;
                            enemies[childId] = childEnemy;
                            healths[childId] = { childEnemy.baseHealth * 0.5f, childEnemy.baseHealth * 0.5f };

                            // ✅ FIX 1: Enable bouncing for the child so it stays on screen
                            bouncingShapes.emplace(childId, BouncingComponent{}); 

                            // ✅ FIX 2: Correct spreadX declaration (removed the duplicate)
                            float spreadX = (i == 0) ? -100.0f : 100.0f;
                            velocities[childId] = { sf::Vector2f(spreadX, 100.0f) };
                        }
                    }
                    
                    if (!sounds.count(enemyId)) sounds.emplace(enemyId, SoundComponent{});
                    sounds.at(enemyId).type = SoundComponent::Type::Explosion;
                }
            }
        }
    }

    // --- 3. Collision: Player vs Enemies ---
    for (auto& [enemyId, enemy] : enemies) {
        if (!actives.count(enemyId) || !actives.at(enemyId).active) continue;

        float distSq = std::pow(positions.at(playerId).position.x - positions.at(enemyId).position.x, 2) +
                       std::pow(positions.at(playerId).position.y - positions.at(enemyId).position.y, 2);
        
        if (distSq < std::pow(shapes.at(playerId).size + shapes.at(enemyId).size, 2)) {
            // Shield/Health Logic
            float dmg = enemy.baseDamage;
            if (shields.count(playerId) && shields.at(playerId).currentShield > 0) {
                shields.at(playerId).currentShield -= dmg;
            } else {
                playerHealths.at(playerId).currentHealth -= dmg;
            }

            actives.at(enemyId).active = false; 

            if (playerHealths.at(playerId).currentHealth <= 0) {
                if (playerLives.count(playerId) && playerLives.at(playerId).lives > 0) {
                    playerLives.at(playerId).lives--;
                    playerHealths.at(playerId).currentHealth = playerHealths.at(playerId).maxHealth;
                    positions.at(playerId).position = sf::Vector2f(400.f, 500.f);
                } else {
                    manager.setState(GameState::GameOver);
                }
            }
        }
    }
}