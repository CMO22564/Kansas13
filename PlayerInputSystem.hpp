#pragma once
#include <vector>
#include <string>
#include <SFML/Window/Event.hpp>
#include "Core.hpp"
#include "GameStateManager.hpp"

class PlayerInputSystem : public System {
public:
    PlayerInputSystem() = default;
    void update(
        const std::vector<EntityId>& entities,
        float dt,
        ComponentMap<PositionComponent>& positions,
        ComponentMap<VelocityComponent>& velocities,
        ComponentMap<PlayerInputComponent>& playerInputs,
        std::vector<EntityId>& entitiesList,
        ComponentMap<RenderComponent>& shapes,
        ComponentMap<ProjectileComponent>& projectiles,
        ComponentMap<ActiveComponent>& activeStates,
        ComponentMap<SoundComponent>& sounds,
        ComponentMap<DamageComponent>& damages
    );
    void handleScoreEntry(sf::Event& event, GameStateManager& manager);
    void resetInitials();
    std::string getCurrentInitials() const { return m_currentInitials; }
    bool isEnteringScore() const { return m_enteringScore; }

private:
    std::string m_currentInitials = "---"; // Current initials being entered
    size_t m_currentInitialIndex = 0; // Current character being edited
    bool m_enteringScore = false; // Flag to indicate score entry mode
};