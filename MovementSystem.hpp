#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Core.hpp"

class MovementSystem {
public:
    MovementSystem();
    void update(const std::vector<EntityId>& entities, 
                ComponentMap<PositionComponent>& positions, 
                ComponentMap<VelocityComponent>& velocities, 
                ComponentMap<BouncingComponent>& bouncingShapes,
                ComponentMap<RenderComponent>& shapes, 
                sf::Time deltaTime);

    // ⭐ FIX: Add the declaration for updateBouncingShapes.
    void updateBouncingShapes(const std::vector<EntityId>& entities, 
                              ComponentMap<PositionComponent>& positions, 
                              ComponentMap<VelocityComponent>& velocities, 
                              ComponentMap<BouncingComponent>& bouncingShapes);
};
