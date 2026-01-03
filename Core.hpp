#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Audio.hpp>
#include "GameStateManager.hpp"

using EntityId = unsigned int;

template<typename T>
using ComponentMap = std::unordered_map<EntityId, T>;

// -- Base System Class --
class System {
public:
    virtual ~System() = default; 
};

// -- Component Structs --
struct PositionComponent { sf::Vector2f position; };
struct VelocityComponent { sf::Vector2f velocity; };

struct PlayerInputComponent {
    float shootCooldown = 0.0f;
    float shootInterval = 0.25f;
    int turretLevel = 0;   
    int credits = 0;       
};

struct PlayerHealthComponent {
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
};

struct ShieldComponent {
    float currentShield = 100.0f;
    float maxShield = 100.0f;
    bool isWarningActive = false;
};

struct HealthComponent {
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
};

struct EnemyComponent {
    float baseDamage;
    float baseSpeed;
    float baseHealth;
    int generation = 1; 
};

struct RenderComponent {
    enum Type { Circle, Square, Triangle, Diamond, Hexagon };
    Type type;
    sf::Color color;
    float size;
    std::unique_ptr<sf::Shape> shape;
};

struct ActiveComponent { bool active = true; };
struct ProjectileComponent {};
struct DamageComponent { float damage = 10.0f; };
struct BouncingComponent {};
struct PlayerLivesComponent { int lives = 3; };

struct SoundComponent {
    enum class Type { Laser, Explosion, ShieldHit, ShieldWarning, PlayerHit, Respawn, GameOver };
    Type type = Type::Explosion;
};

inline unsigned int getNextEntityId() {
    static unsigned int nextId = 1;
    return nextId++;
}

// --- ADD THIS AT THE BOTTOM ---
/**
 * @brief Resets player-specific components (Health, Shield, Lives) 
 * to their initial starting values.
 */
inline void resetPlayerComponents(
    EntityId playerId,
    ComponentMap<PlayerHealthComponent>& playerHealths,
    ComponentMap<ShieldComponent>& shields,
    ComponentMap<PlayerLivesComponent>& playerLives) 
{
    if (playerHealths.count(playerId)) {
        playerHealths.at(playerId).currentHealth = playerHealths.at(playerId).maxHealth;
    }
    if (shields.count(playerId)) {
        shields.at(playerId).currentShield = shields.at(playerId).maxShield;
        shields.at(playerId).isWarningActive = false;
    }
    if (playerLives.count(playerId)) {
        playerLives.at(playerId).lives = 3;
    }
}