#include "PlayerInputSystem.hpp"
#include "Core.hpp"
#include "GameStateManager.hpp"
#include <iostream>
#include <cmath>
#include <limits> // For std::numeric_limits

void PlayerInputSystem::update(
    const std::vector<EntityId>& entities,
    float dt,
    ComponentMap<PositionComponent>& positions,
    ComponentMap<VelocityComponent>& velocities,
    ComponentMap<PlayerInputComponent>& playerInputs,
    std::vector<EntityId>& entitiesList,
    ComponentMap<RenderComponent>& shapes,
    ComponentMap<ProjectileComponent>& projectiles,
    ComponentMap<ActiveComponent>& activeStates,
    ComponentMap<SoundComponent>& sounds,
    ComponentMap<DamageComponent>& damages) {
    
    if (GameStateManager::getInstance().getState() != GameState::Running) {
        return;
    }

    for (EntityId entityId : entities) {
        if (positions.count(entityId) && velocities.count(entityId) && playerInputs.count(entityId)) {
            auto& velocity = velocities.at(entityId).velocity;
            auto& input = playerInputs.at(entityId);
            
            // Player movement logic
            sf::Vector2f moveDirection(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                moveDirection.x -= 1.0f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                moveDirection.x += 1.0f;
            }

            float speed = 500.0f;
            velocity = moveDirection * speed;
            
            // Projectile shooting logic
            const float SHOOT_COOLDOWN = 0.00f;
            input.shootCooldown += dt;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && input.shootCooldown >= SHOOT_COOLDOWN) {
                // Find the closest enemy to target
                EntityId closestEnemy = 0;
                float minDistance = std::numeric_limits<float>::max();
                
                sf::Vector2f playerPos = positions.at(entityId).position;

                for (EntityId currentEntity : entitiesList) {
                    if (currentEntity != entityId && activeStates.count(currentEntity) && activeStates.at(currentEntity).active) {
                        if (positions.count(currentEntity) && shapes.count(currentEntity)) {
                            sf::Vector2f enemyPos = positions.at(currentEntity).position;
                            float dist = std::sqrt(std::pow(playerPos.x - enemyPos.x, 2) + std::pow(playerPos.y - enemyPos.y, 2));
                            
                            if (dist < minDistance) {
                                minDistance = dist;
                                closestEnemy = currentEntity;
                            }
                        }
                    }
                }

                // If a closest enemy was found, shoot a projectile at it
                if (closestEnemy != 0) {
                    EntityId projectileId = getNextEntityId();
                    entitiesList.push_back(projectileId);
                    
                    positions.emplace(projectileId, PositionComponent{playerPos});
                    
                    sf::Vector2f enemyPos = positions.at(closestEnemy).position;
                    sf::Vector2f direction = enemyPos - playerPos;
                    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                    
                    if (length > 0) {
                        sf::Vector2f normalizedDirection = direction / length;
                        float projectileSpeed = 800.0f;
                        velocities.emplace(projectileId, VelocityComponent{normalizedDirection * projectileSpeed});
                    } else {
                        velocities.emplace(projectileId, VelocityComponent{sf::Vector2f(0.f, -800.0f)});
                    }

                    RenderComponent projectileRender;
                    projectileRender.type = RenderComponent::Type::Circle;
                    projectileRender.color = sf::Color::Yellow;
                    projectileRender.size = 5.0f;
                    
                    shapes.emplace(projectileId, std::move(projectileRender));
                    projectiles.emplace(projectileId, ProjectileComponent{});
                    activeStates.emplace(projectileId, ActiveComponent{true});
                    sounds.emplace(projectileId, SoundComponent{SoundComponent::Type::Laser});
                    damages.emplace(projectileId, DamageComponent{10.0f});

                    input.shootCooldown = 0.00f;
                } else {
                    // Fallback to the original behavior if no enemy is found
                    EntityId projectileId = getNextEntityId();
                    entitiesList.push_back(projectileId);

                    positions.emplace(projectileId, PositionComponent{playerPos});
                    velocities.emplace(projectileId, VelocityComponent{sf::Vector2f(0.f, -800.0f)});
                    
                    RenderComponent projectileRender;
                    projectileRender.type = RenderComponent::Type::Circle;
                    projectileRender.color = sf::Color::Yellow;
                    projectileRender.size = 5.0f;
                    
                    shapes.emplace(projectileId, std::move(projectileRender));
                    projectiles.emplace(projectileId, ProjectileComponent{});
                    activeStates.emplace(projectileId, ActiveComponent{true});
                    sounds.emplace(projectileId, SoundComponent{SoundComponent::Type::Laser});
                    damages.emplace(projectileId, DamageComponent{10.0f});

                    input.shootCooldown = 0.05f;
                }
            }
        }
    }
}
