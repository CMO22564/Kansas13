#include "PlayerInputSystem.hpp"
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

void PlayerInputSystem::update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<PlayerInputComponent>& playerInputs, std::vector<EntityId>& entitiesList, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds, ComponentMap<DamageComponent> damageValues) {
    for (EntityId entity : entities) {
        if (positions.count(entity) && velocities.count(entity) && playerInputs.count(entity)) {
            auto& velocity = velocities[entity].velocity;
            auto& input = playerInputs[entity];

            // Reset velocity
            velocity = sf::Vector2f(0.f, 0.f);

            // Handle movement (e.g., left/right with A/D or arrow keys)
            float speed = 500.f; // Player speed in pixels/second
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                velocity.x = -speed;
                input.isMovingLeft = true;
            } else {
                input.isMovingLeft = false;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                velocity.x = speed;
                input.isMovingRight = true;
            } else {
                input.isMovingRight = false;
            }

            // Handle shooting (e.g., spacebar)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                if (!input.isShooting) { // Prevent continuous spawning
                    input.isShooting = true;
                    // Spawn projectile at player's position
                    spawnProjectile(positions[entity].position, positions, velocities, shapes, projectiles, activeStates, sounds, entitiesList, damageValues);
                }
            } else {
                input.isShooting = false;
            }
        }
    }
}

void PlayerInputSystem::spawnProjectile(
    const sf::Vector2f& startPosition, 
    ComponentMap<PositionComponent>& positions, 
    ComponentMap<VelocityComponent>& velocities, 
    ComponentMap<ShapeComponent>& shapes,
    ComponentMap<ProjectileComponent>& projectiles, 
    ComponentMap<ActiveComponent>& activeStates,
    ComponentMap<SoundComponent>& sounds,
    std::vector<EntityId>& entities,
    ComponentMap<DamageComponent>& damageValues) {
    
    EntityId projectileId = getNextEntityId();
    entities.push_back(projectileId);
    
    positions.emplace(projectileId, PositionComponent{ startPosition });
    velocities.emplace(projectileId, VelocityComponent{ sf::Vector2f(0.f, -500.f) });
    
    // Correctly initialize ShapeComponent with data, leaving the shape pointer null.
    shapes.emplace(projectileId, ShapeComponent{ 
        ShapeComponent::Type::Circle, // Set the type
        sf::Color::Red,               // Set the color
        sf::Vector2f(10.f, 10.f),       // Set the size
        nullptr                       // The shape pointer is initially null
    });
    
    projectiles.emplace(projectileId, ProjectileComponent{});
    activeStates.emplace(projectileId, ActiveComponent{});
    damageValues.emplace(projectileId, DamageComponent{ 10.f });
    sounds.emplace(projectileId, SoundComponent{ SoundComponent::Type::Laser });
}
