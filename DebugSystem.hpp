#pragma once

#include "Core.hpp"
#include <vector>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>

class DebugSystem {
public:
    void update(int score,
                const ComponentMap<PlayerHealthComponent>& playerHealths,
                const ComponentMap<ShieldComponent>& shields,
                const ComponentMap<PlayerLivesComponent>& playerLives,
                const std::vector<EntityId>& entities, // Add entities
                const ComponentMap<PositionComponent>& positions, // Add positions
                const ComponentMap<ShapeComponent>& shapes, // Add shapes
                const ComponentMap<VelocityComponent>& velocities); // Add velocities

    void handleEvent(const sf::Event& event, bool& showImGui, GameState& gameState);
    
    // Member variable to control shape visibility
    bool showShapes = true; 
};
