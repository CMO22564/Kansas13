#include "RenderSystem.hpp"
#include "Core.hpp" // Contains RenderComponent, PositionComponent, etc.
#include <iostream>
#include <SFML/Graphics.hpp> // Required for sf::RenderWindow, sf::CircleShape, etc.
#include <memory>          // FIX: Required for std::make_unique

// CRITICAL FIX: Removed invisible characters and ensured parameter name consistency
void RenderSystem::update(sf::RenderWindow& window,
                          const std::vector<EntityId>& entities,
                          const ComponentMap<PositionComponent>& positions,
                          ComponentMap<RenderComponent>& renderables, // FIX: Renamed 'shapes' to 'renderables' for consistency with usage below
                          const ComponentMap<ActiveComponent>& activeStates) {
    
    for (EntityId id : entities) {
        // Check for active entities with Position and Render Components
        // FIX: Replaced 'shapes.count(id)' with 'renderables.count(id)' if that was intended
        if (positions.count(id) && renderables.count(id) && activeStates.count(id) && activeStates.at(id).active) {
            const auto& position = positions.at(id);
            auto& renderComponent = renderables.at(id); // Access RenderComponent

            // CRITICAL FIX: Create the shape if it doesn't exist
            if (!renderComponent.shape) {
                // Use the component's Type and single float size to define the shape
                if (renderComponent.type == RenderComponent::Type::Circle) {
                    // Circle radius is the size
                    renderComponent.shape = std::make_unique<sf::CircleShape>(renderComponent.size);
                } else if (renderComponent.type == RenderComponent::Type::Square) {
                    // Create a square using the single size float (size * 2 is the full width/height)
                    renderComponent.shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(renderComponent.size * 2.f, renderComponent.size * 2.f));
                } else if (renderComponent.type == RenderComponent::Type::Triangle) {
                    // Create a 3-point shape (triangle) using the size as the radius
                    renderComponent.shape = std::make_unique<sf::CircleShape>(renderComponent.size, 3);
                }

                // Set properties that only need to be set once
                if (renderComponent.shape) {
                    renderComponent.shape->setFillColor(renderComponent.color);
                    
                    // RenderSystem.cpp (New Line 40)
                    renderComponent.shape->setOrigin(sf::Vector2f(renderComponent.size, renderComponent.size));
                }
            }
            
            // Draw the shape
            if (renderComponent.shape) {
                renderComponent.shape->setPosition(position.position);
                window.draw(*renderComponent.shape);
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
