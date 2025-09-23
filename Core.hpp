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

struct ShapeComponent {
    enum Type { Circle, Square, Triangle };
    Type type;
    sf::Color color;
    sf::Vector2f size;
    // CRITICAL FIX: Add a unique_ptr to manage the SFML shape object
    std::unique_ptr<sf::Shape> shape; 
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
