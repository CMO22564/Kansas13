#pragma once
#include <vector>
#include "Core.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class PlayerInputSystem {
public:
    void update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<PlayerInputComponent>& playerInputs, std::vector<EntityId>& entitiesList, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds, ComponentMap<DamageComponent> damageValues);
    
    void spawnProjectile(const sf::Vector2f& startPosition, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds, std::vector<EntityId>& entities, ComponentMap<DamageComponent>& damageValues);
    };
