#pragma once

#include <vector>
#include <list>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <unordered_map>
#include "Core.hpp"
#include "PlayerInputSystem.hpp"
#include "MovementSystem.hpp"
#include "ProjectileSystem.hpp"
#include "DamageSystem.hpp"
#include "CleanUpSystem.hpp"
#include "RenderSystem.hpp"
#include "SoundSystem.hpp"
#include "DebugSystem.hpp"
#include "BouncingShapeSystem.hpp"
#include "CombatSystem.hpp" // Now includes CombatSystem.hpp

class PlayerInputSystem {
private:
    bool _movingLeft = false;
    bool _movingRight = false;
    float _playerSpeed = 250.0f;
    void spawnProjectile(const sf::Vector2f& position, std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds);
public:
    void handleEvent(const sf::Event& event, GameState& gameState, PositionComponent& playerPosition, std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds);
    void update(PositionComponent& playerPosition, VelocityComponent& playerVelocity);
};

class MovementSystem {
public:
    void update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, sf::Time deltaTime);
};

class BouncingShapeSystem {
public:
    void update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<BouncingShapeComponent>& bouncingShapes);
};

class ProjectileSystem {
public:
    void update(std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<BouncingShapeComponent>& bouncingShapes, ComponentMap<SoundComponent>& sounds, int& score);
private:
    sf::FloatRect getRect(const PositionComponent& pos, const ShapeComponent& shape);
};

class DamageSystem {
private:
    sf::FloatRect getRect(const PositionComponent& pos, const ShapeComponent& shape);
public:
    void update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<ShapeComponent>& shapes, ComponentMap<ActiveComponent>& activeStates, ComponentMap<DamageComponent>& damageValues, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<SoundComponent>& sounds, ComponentMap<BouncingShapeComponent>& bouncingShapes, int& score, ComponentMap<PlayerLivesComponent>& playerLives, GameState& gameState);
};

class CleanUpSystem {
public:
    void update(std::vector<EntityId>& entities, ComponentMap<ActiveComponent>& activeStates, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<DamageComponent>& damageValues, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<BouncingShapeComponent>& bouncingShapes);
};

class RenderSystem {
public:
    void update(sf::RenderWindow& window, const std::vector<EntityId>& entities, const ComponentMap<PositionComponent>& positions, const ComponentMap<ShapeComponent>& shapes);
    void renderHealthBar(sf::RenderWindow& window, sf::Vector2f position, float currentHealth, float maxHealth);
    void renderShieldBar(sf::RenderWindow& window, sf::Vector2f position, float currentShield, float maxShield);
};

class SoundSystem {
private:
    sf::SoundBuffer laserBuffer;
    sf::SoundBuffer explosionBuffer;
    sf::SoundBuffer shieldBuffer;
    sf::SoundBuffer playerHitBuffer;
    std::list<sf::Sound> activeSounds;
public:
    SoundSystem();
    void update(ComponentMap<SoundComponent>& sounds);
};

class DebugSystem {
public:
    void handleEvent(const sf::Event& event, bool& showImGui, GameState& gameState);
};
