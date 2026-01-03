#pragma once
#include <vector>
#include <string>
#include <SFML/Window/Event.hpp>
#include "Core.hpp"

class PlayerInputSystem : public System {
public:
    PlayerInputSystem() = default;

    void update(
        const std::vector<EntityId>& entities,
        const ComponentMap<EnemyComponent>& enemyComponents, // 🌟 Fixed Order
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

    void handleScoreEntry(const sf::Event& event, GameStateManager& manager);
    void resetInitials();

private:
    EntityId findNearestEnemy(
        sf::Vector2f playerPos, 
        const std::vector<EntityId>& entitiesList,
        const ComponentMap<PositionComponent>& positions,
        const ComponentMap<ActiveComponent>& activeStates,
        const ComponentMap<EnemyComponent>& enemyComponents
    );

    std::string m_currentInitials = "---";
    size_t m_currentInitialIndex = 0;
    bool m_enteringScore = false;
};