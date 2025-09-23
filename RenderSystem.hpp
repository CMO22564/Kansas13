#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Core.hpp"

class RenderSystem {
public:
    void update(sf::RenderWindow& window,
                const std::vector<EntityId>& entities,
                const ComponentMap<PositionComponent>& positions,
                ComponentMap<ShapeComponent>& shapes,
                const ComponentMap<ActiveComponent>& activeStates); // ⭐ FIX: Added activeStates parameter

    void renderHealthBar(sf::RenderWindow& window, sf::Vector2f position, float currentHealth, float maxHealth);
    void renderShieldBar(sf::RenderWindow& window, sf::Vector2f position, float currentShield, float maxShield);
};
