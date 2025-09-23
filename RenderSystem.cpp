#include "RenderSystem.hpp"
#include "Core.hpp"
#include <iostream>

void RenderSystem::update(sf::RenderWindow& window,
                          const std::vector<EntityId>& entities,
                          const ComponentMap<PositionComponent>& positions,
                          ComponentMap<ShapeComponent>& shapes, // Must be non-const now
                          const ComponentMap<ActiveComponent>& activeStates) {
    
    for (EntityId id : entities) {
        if (positions.count(id) && shapes.count(id) && activeStates.count(id) && activeStates.at(id).active) {
            const auto& position = positions.at(id);
            auto& shapeComponent = shapes.at(id);

            // CRITICAL FIX: Create the shape if it doesn't exist
            if (!shapeComponent.shape) {
                if (shapeComponent.type == ShapeComponent::Type::Circle) {
                    shapeComponent.shape = std::make_unique<sf::CircleShape>(shapeComponent.size.x / 2.f);
                } else if (shapeComponent.type == ShapeComponent::Type::Square) {
                    shapeComponent.shape = std::make_unique<sf::RectangleShape>(shapeComponent.size);
                } else if (shapeComponent.type == ShapeComponent::Type::Triangle) {
                    shapeComponent.shape = std::make_unique<sf::CircleShape>(shapeComponent.size.x / 2.f, 3);
                }

                // Set properties that only need to be set once
                if (shapeComponent.shape) {
                    shapeComponent.shape->setFillColor(shapeComponent.color);
                    // ⭐ FIX: Change to use sf::Vector2f
                    shapeComponent.shape->setOrigin(sf::Vector2f(shapeComponent.size.x / 2.f, shapeComponent.size.y / 2.f));
                }
            }
            
            // CRITICAL FIX: Use the '->' operator to access the unique_ptr's members
            if (shapeComponent.shape) {
                shapeComponent.shape->setPosition(position.position);
                window.draw(*shapeComponent.shape);
            }
        }
    }
}

// Draw a health bar
void RenderSystem::renderHealthBar(sf::RenderWindow& window, sf::Vector2f position,
                                     float currentHealth, float maxHealth) {
    float barWidth = 100.0f;
    float barHeight = 10.0f;

    sf::RectangleShape background(sf::Vector2f(barWidth, barHeight));
    background.setFillColor(sf::Color(50, 50, 50));
    background.setPosition(position);
    window.draw(background);

    float healthPercentage = currentHealth / maxHealth;
    sf::RectangleShape foreground(sf::Vector2f(barWidth * healthPercentage, barHeight));
    foreground.setFillColor(sf::Color::Green);
    foreground.setPosition(position);
    window.draw(foreground);
}

// Draw a shield bar
void RenderSystem::renderShieldBar(sf::RenderWindow& window, sf::Vector2f position,
                                     float currentShield, float maxShield) {
    float barWidth = 100.0f;
    float barHeight = 10.0f;

    sf::RectangleShape background(sf::Vector2f(barWidth, barHeight));
    background.setFillColor(sf::Color(50, 50, 50));
    background.setPosition(position);
    window.draw(background);

    float shieldPercentage = currentShield / maxShield;
    sf::RectangleShape foreground(sf::Vector2f(barWidth * shieldPercentage, barHeight));
    foreground.setFillColor(sf::Color::Cyan);
    foreground.setPosition(position);
    window.draw(foreground);
}
