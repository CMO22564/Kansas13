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
// ⭐ CRITICAL FIX: Add the base System class definition here
// This resolves the "expected class-name before '{' token" error in EnemySpawnSystem.hpp
class System {
public:
    // A virtual destructor is good practice for a base class
    virtual ~System() = default; 
    
    // Systems typically have an update method, even if it's not pure virtual
    // (Actual update methods will be defined in derived classes like EnemySpawnSystem)
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
    int generation = 0; // Default generation is 0
};

// Core.hpp (around lines 56-65)

// NOTE: I'm replacing your existing ShapeComponent with this new RenderComponent.
// This component holds the data required for the RenderSystem to draw the entity.
struct RenderComponent {
    // Enum must be defined inside or accessible globally.
    enum Type { Circle, Square, Triangle };
    Type type;
    sf::Color color;
    // CRITICAL: Change size from Vector2f to a single float 
    float size; 
    
    // CRITICAL FIX: The RenderSystem needs the SFML object stored here
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
    bool isMovingLeft = false;
    bool isMovingRight = false;
    bool isShooting = false;
};

struct ProjectileComponent {};

struct CombatComponent {
    float damage = 10.0f;
};

// ⭐ FIX: Add the Type enum inside the SoundComponent struct
struct SoundComponent {
    enum class Type {
        Laser,
        Explosion,
        ShieldHit,
        PlayerHit
        };
     Type type;
    // Add other members as needed, e.g., to control playback
};

// Global counter for entities
inline unsigned int getNextEntityId() {
    static unsigned int nextId = 1;
    return nextId++;
}
