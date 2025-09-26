#include "PlayerInputSystem.hpp"
#include "Core.hpp" // Make sure this is included for Component definitions
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

// Corrected update function definition
void PlayerInputSystem::update(const std::vector<EntityId>& entities, // FIX: entities is first
                               float dt, // FIX: dt is second
                               ComponentMap<PositionComponent>& positions, 
                               ComponentMap<VelocityComponent>& velocities, 
                               ComponentMap<PlayerInputComponent>& playerInputs, 
                               std::vector<EntityId>& entitiesList, 
                               ComponentMap<RenderComponent>& shapes, 
                               ComponentMap<ProjectileComponent>& projectiles, 
                               ComponentMap<ActiveComponent>& activeStates, 
                               ComponentMap<SoundComponent>& sounds, 
                               ComponentMap<DamageComponent> damageValues) {
    
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
                    // NOTE: The header likely uses shoot/spawnProjectile, so I'll keep the call as is
                    spawnProjectile(positions[entity].position, positions, velocities, shapes, projectiles, activeStates, sounds, entitiesList, damageValues);
                }
            } else {
                input.isShooting = false;
            }
        }
    }
}

// Corrected spawnProjectile function definition
void PlayerInputSystem::spawnProjectile(
    const sf::Vector2f& startPosition, 
    ComponentMap<PositionComponent>& positions, 
    ComponentMap<VelocityComponent>& velocities, 
    ComponentMap<RenderComponent>& shapes, // FIX: Use RenderComponent
    ComponentMap<ProjectileComponent>& projectiles, 
    ComponentMap<ActiveComponent>& activeStates,
    ComponentMap<SoundComponent>& sounds,
    std::vector<EntityId>& entities,
    ComponentMap<DamageComponent>& damageValues) {
    
    EntityId projectileId = getNextEntityId();
    entities.push_back(projectileId);
    
    positions.emplace(projectileId, PositionComponent{ startPosition });
    velocities.emplace(projectileId, VelocityComponent{ sf::Vector2f(0.f, -500.f) });
    
    // Create the new RenderComponent object
    RenderComponent projectileRender;
    
    // FIX: Explicitly set the new members for RenderComponent
    projectileRender.type = RenderComponent::Type::Circle; // Set the type
    projectileRender.color = sf::Color::Yellow;           // Projectiles are typically yellow/white
    projectileRender.size = 5.0f;                         // Use single float size (radius)
    // projectileRender.shape is a unique_ptr and defaults to nullptr, which is correct here.
    
    // Store the component using the correct type and move the initialized data
    shapes.emplace(projectileId, std::move(projectileRender));
    
    projectiles.emplace(projectileId, ProjectileComponent{});
    activeStates.emplace(projectileId, ActiveComponent{ true }); // Initialize active state to true
    damageValues.emplace(projectileId, DamageComponent{ 20.f }); // Use 20.f damage
    sounds.emplace(projectileId, SoundComponent{ SoundComponent::Type::Laser });
}
