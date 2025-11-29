#include "RenderSystem.hpp"
#include "Core.hpp" // Contains RenderComponent, PositionComponent, etc.
#include <iostream>
#include <SFML/Graphics.hpp> // Required for sf::RenderWindow, sf::CircleShape, etc.
#include <SFML/System/Angle.hpp> // 🌟 OPTIONAL: Add this line to explicitly include Angle utilities
#include <memory>          // FIX: Required for std::make_unique


void RenderSystem::update(sf::RenderWindow& window,
                          const std::vector<EntityId>& entities,
                          const ComponentMap<PositionComponent>& positions,
                          ComponentMap<RenderComponent>& renderables, // FIX: Renamed 'shapes' to 'renderables' for consistency with usage below
                          const ComponentMap<ActiveComponent>& activeStates) {
    
    for (EntityId id : entities) {
        // Check for active entities with Position and Render Components
      
        if (positions.count(id) && renderables.count(id) && activeStates.count(id) && activeStates.at(id).active) {
            const auto& position = positions.at(id);
            auto& renderComponent = renderables.at(id); // Access RenderComponent

           
            if (!renderComponent.shape) {
                
                // 1. SHAPE CREATION (ONLY) inside the switch
                switch (renderComponent.type) {
                    case RenderComponent::Circle:
                        renderComponent.shape = std::make_unique<sf::CircleShape>(renderComponent.size);
                        break;
                    case RenderComponent::Square:
                        renderComponent.shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(renderComponent.size * 2, renderComponent.size * 2));
                        break;
                    case RenderComponent::Triangle:
                        renderComponent.shape = std::make_unique<sf::CircleShape>(renderComponent.size, 3);
                        break;
                    case RenderComponent::Hexagon:
                        renderComponent.shape = std::make_unique<sf::CircleShape>(renderComponent.size, 6); 
                        break;
                    case RenderComponent::Diamond: 
                        // 🌟 FIX: Use sf::RectangleShape for a reliable, rotatable square base.
                        // Size is (2 * radius, 2 * radius) to make a centered square.
                        renderComponent.shape = std::make_unique<sf::RectangleShape>(
                            sf::Vector2f(renderComponent.size * 2.f, renderComponent.size * 2.f));
                        break;
                    default:
                        renderComponent.shape = std::make_unique<sf::CircleShape>(renderComponent.size);
                        break;
                } // End of switch
                
                // 2. COMMON SETUP (ORIGIN and FILL COLOR) for ALL shapes
                // This MUST happen BEFORE the rotation for Diamond to be centered.
                if (renderComponent.shape) {
                    renderComponent.shape->setOrigin(sf::Vector2f(renderComponent.size, renderComponent.size));
                    renderComponent.shape->setFillColor(renderComponent.color);
                }

                // 3. DIAMOND SPECIFIC SETUP (ROTATION)
                if (renderComponent.type == RenderComponent::Type::Diamond) {
                    renderComponent.shape->setRotation(sf::degrees(45.f)); 
                }

            } // End of if (!renderComponent.shape)

            // ... (rest of the drawing logic)
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
