#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameStateManager.hpp"

using EntityId = unsigned int;

// Templated ComponentMap to hold components for entities
template<typename T>
using ComponentMap = std::unordered_map<EntityId, T>;

// -- Base System Class --
class System {
public:
    virtual ~System() = default; 
};

// -- Component Structs --

struct PlayerHealthComponent {
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
};

struct ShieldComponent {
    float currentShield = 100.0f;
    float maxShield = 100.0f;
};

struct PlayerLivesComponent {
    int lives = 3;
};

struct ActiveComponent {
    bool active = true;
};

struct DamageComponent {
    float damage = 10.0f;
};

struct BouncingComponent {
    int generation = 0;
};

struct RenderComponent {
    enum Type { Circle, Square, Triangle };
    Type type;
    sf::Color color;
    float size; 
    std::unique_ptr<sf::Shape> shape; 
};

struct HealthComponent {
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
};

struct PositionComponent {
    sf::Vector2f position;
};

struct VelocityComponent {
    sf::Vector2f velocity;
};

struct PlayerInputComponent {
    float shootCooldown = 0.0f;
};

struct ProjectileComponent {};

struct CombatComponent {
    float damage = 10.0f;
};

struct SoundComponent {
    enum class Type {
        Laser,
        Explosion,
        ShieldHit,
        PlayerHit,
        Spawn
        };
      Type type;
};

// NEW FEATURE: Enemy Tag Component
struct EnemyComponent {};

// Global counter for entities
inline unsigned int getNextEntityId() {
    static unsigned int nextId = 1;
    return nextId++;
}
