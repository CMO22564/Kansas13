#include "MovementSystem.hpp"
#include <iostream>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp> // Include this header for the CircleShape class



MovementSystem::MovementSystem() {
    // Default constructor
}

void MovementSystem::update(const std::vector<EntityId>& entities,
                            ComponentMap<PositionComponent>& positions,
                            ComponentMap<VelocityComponent>& velocities,
                            ComponentMap<BouncingComponent>& bouncingComponents,
                            ComponentMap<RenderComponent>& shapes,
                            sf::Time deltaTime) {
    for (EntityId entity : entities) {
        if (positions.count(entity) && velocities.count(entity)) {
            // Update position
            positions.at(entity).position += velocities.at(entity).velocity * deltaTime.asSeconds();
        }

        // Handle bouncing for entities with BouncingComponent
        if (bouncingComponents.count(entity) && shapes.count(entity)) {
            auto& position = positions.at(entity).position;
            auto& velocity = velocities.at(entity).velocity;
            auto& shapeComponent = shapes.at(entity);
            
            // Check if the shape exists and is a circle
            if (shapeComponent.shape && shapeComponent.type == RenderComponent::Type::Circle) {
                // Downcast the sf::Shape pointer to a sf::CircleShape pointer
                sf::CircleShape* circleShape = dynamic_cast<sf::CircleShape*>(shapeComponent.shape.get());
                
                if (circleShape) {
                    float radius = circleShape->getRadius();
                    float shapeHalfSize = radius;
                    
                    // Left wall bounce
                    if (position.x - shapeHalfSize < 0) {
                        position.x = shapeHalfSize;
                        velocity.x = -velocity.x;
                    }
                    // Right wall bounce
                    if (position.x + shapeHalfSize > 800) {
                        position.x = 800 - shapeHalfSize;
                        velocity.x = -velocity.x;
                    }
                    // Top wall bounce
                    if (position.y - shapeHalfSize < 0) {
                        position.y = shapeHalfSize;
                        velocity.y = -velocity.y;
                    }
                    // Bottom wall bounce
                    if (position.y + shapeHalfSize > 600) {
                        position.y = 600 - shapeHalfSize;
                        velocity.y = -velocity.y;
                    }
                }
            }
        }
    }
}
