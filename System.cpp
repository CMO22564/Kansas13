#include <SFML/Graphics/Rect.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include "System.hpp"
#include <iostream>
#include <random>
#include <variant>
#include "Core.hpp"
#include <algorithm>
#include <optional>
#include <list>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

// PlayerInputSystem implementation
void PlayerInputSystem::handleEvent(const sf::Event& event, GameState& gameState, PositionComponent& playerPosition, std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds) {
    if (gameState != GameState::Running) {
        return;
    }
    
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::A) {
            _movingLeft = true;
        }
        if (keyEvent->code == sf::Keyboard::Key::D) {
            _movingRight = true;
        }
        if (keyEvent->code == sf::Keyboard::Key::Space) {
            spawnProjectile(playerPosition.position, entities, positions, velocities, shapes, projectiles, activeStates, sounds);
        }
    } else if (const auto* keyEvent = event.getIf<sf::Event::KeyReleased>()) {
        if (keyEvent->code == sf::Keyboard::Key::A) {
            _movingLeft = false;
        }
        if (keyEvent->code == sf::Keyboard::Key::D) {
            _movingRight = false;
        }
    }
}

void PlayerInputSystem::update(PositionComponent& playerPosition, VelocityComponent& playerVelocity) {
    playerVelocity.velocity.x = 0;
    if (_movingLeft) {
        playerVelocity.velocity.x -= _playerSpeed;
    }
    if (_movingRight) {
        playerVelocity.velocity.x += _playerSpeed;
    }
}

void PlayerInputSystem::spawnProjectile(const sf::Vector2f& position, std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<SoundComponent>& sounds) {
    EntityId projectileId = getNextEntityId();
    entities.push_back(projectileId);
    positions[projectileId] = {position};
    velocities[projectileId] = {sf::Vector2f(0.0f, -600.0f)};
    shapes[projectileId] = {ShapeType::Square, sf::Color::Yellow, sf::Vector2f(5.0f, 20.0f)};
    projectiles[projectileId] = {};
    activeStates[projectileId] = {true};
    sounds[projectileId] = {SoundType::PlayerShot};
}

// MovementSystem implementation
void MovementSystem::update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, sf::Time deltaTime) {
    for (EntityId id : entities) {
        if (positions.count(id) && velocities.count(id)) {
            positions.at(id).position += velocities.at(id).velocity * deltaTime.asSeconds();
        }
    }
}

// BouncingShapeSystem implementation
void BouncingShapeSystem::update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<BouncingShapeComponent>& bouncingShapes) {
    for (EntityId id : entities) {
        if (positions.count(id) && velocities.count(id) && shapes.count(id) && bouncingShapes.count(id)) {
            sf::Vector2f shapeSize = shapes.at(id).size;
            sf::Vector2f shapePos = positions.at(id).position;
            sf::Vector2f& shapeVel = velocities.at(id).velocity;

            // Bounce off top and bottom
            if (shapePos.y - shapeSize.y / 2.0f < 0 || shapePos.y + shapeSize.y / 2.0f > 600) {
                shapeVel.y = -shapeVel.y;
            }
            // Bounce off left and right
            if (shapePos.x - shapeSize.x / 2.0f < 0 || shapePos.x + shapeSize.x / 2.0f > 800) {
                shapeVel.x = -shapeVel.x;
            }
        }
    }
}

// ProjectileSystem implementation
void ProjectileSystem::update(std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<ActiveComponent>& activeStates, ComponentMap<BouncingShapeComponent>& bouncingShapes, ComponentMap<SoundComponent>& sounds, int& score) {
    for (EntityId projectileId : entities) {
        if (!projectiles.count(projectileId)) continue;
        
        for (EntityId bouncingId : entities) {
            if (!bouncingShapes.count(bouncingId) || !activeStates.at(bouncingId).active) continue;

            sf::FloatRect projectileRect = getRect(positions.at(projectileId), shapes.at(projectileId));
            sf::FloatRect bouncingRect = getRect(positions.at(bouncingId), shapes.at(bouncingId));

            std::optional<sf::FloatRect> intersection = projectileRect.findIntersection(bouncingRect);
            if (intersection.has_value()) {
                activeStates.at(bouncingId).active = false;
                activeStates.at(projectileId).active = false;
                sounds[projectileId] = {SoundType::EnemyHit};
                score += 100;
                break;
            }
        }
    }
}

sf::FloatRect ProjectileSystem::getRect(const PositionComponent& pos, const ShapeComponent& shape) {
    return sf::FloatRect(sf::Vector2f(pos.position.x - shape.size.x / 2.0f, pos.position.y - shape.size.y / 2.0f), shape.size);
}

// DamageSystem implementation
void DamageSystem::update(const std::vector<EntityId>& entities, ComponentMap<PositionComponent>& positions, ComponentMap<ShapeComponent>& shapes, ComponentMap<ActiveComponent>& activeStates, ComponentMap<DamageComponent>& damageValues, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<SoundComponent>& sounds, ComponentMap<BouncingShapeComponent>& bouncingShapes, int& score, ComponentMap<PlayerLivesComponent>& playerLives, GameState& gameState) {

    EntityId playerId = 1;
    EntityId shieldId = 2;

    if (!playerHealths.count(playerId) || !shields.count(shieldId)) {
        return;
    }

    // Loop through all entities that have a DamageComponent
    for (EntityId damagingId : entities) {
        if (!damageValues.count(damagingId) || !activeStates.at(damagingId).active) {
            continue;
        }

        // IMPORTANT: Skip if the damaging entity is the player or shield
        if (damagingId == playerId || damagingId == shieldId) {
            continue;
        }

        sf::FloatRect damagingRect = getRect(positions.at(damagingId), shapes.at(damagingId));

        bool hitSomething = false;

        // Check for collision with the shield
        if (shields.count(shieldId) && activeStates.at(shieldId).active) {
            sf::FloatRect shieldRect = getRect(positions.at(shieldId), shapes.at(shieldId));
            std::optional<sf::FloatRect> intersection = damagingRect.findIntersection(shieldRect);
            if (intersection.has_value()) {
                shields.at(shieldId).currentHealth -= damageValues.at(damagingId).damage;
                sounds[damagingId] = {SoundType::ShieldHit};
                activeStates.at(damagingId).active = false;
                score += 5;
                hitSomething = true;
            }
        }

        // Check for collision with the player, but only if the shield wasn't hit
        if (!hitSomething) {
            if (playerHealths.count(playerId) && activeStates.at(playerId).active) {
                sf::FloatRect playerRect = getRect(positions.at(playerId), shapes.at(playerId));
                std::optional<sf::FloatRect> intersection = damagingRect.findIntersection(playerRect);
                if (intersection.has_value()) {
                    playerHealths.at(playerId).currentHealth -= damageValues.at(damagingId).damage;
                    sounds[damagingId] = {SoundType::PlayerHit};
                    activeStates.at(damagingId).active = false;
                    if (playerHealths.at(playerId).currentHealth <= 0) {
                        playerLives.at(playerId).currentLives--;
                        if (playerLives.at(playerId).currentLives <= 0) {
                            gameState = GameState::GameOver;
                        } else {
                            playerHealths.at(playerId).currentHealth = playerHealths.at(playerId).maxHealth;
                        }
                    }
                }
            }
        }
    }
}


sf::FloatRect DamageSystem::getRect(const PositionComponent& pos, const ShapeComponent& shape) {
    return sf::FloatRect(sf::Vector2f(pos.position.x - shape.size.x / 2.0f, pos.position.y - shape.size.y / 2.0f), shape.size);
}


// CleanUpSystem implementation
void CleanUpSystem::update(std::vector<EntityId>& entities, ComponentMap<ActiveComponent>& activeStates, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<DamageComponent>& damageValues, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<BouncingShapeComponent>& bouncingShapes) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&](EntityId id) {
        if (activeStates.count(id) && !activeStates.at(id).active) {
            positions.erase(id);
            velocities.erase(id);
            shapes.erase(id);
            projectiles.erase(id);
            damageValues.erase(id);
            shields.erase(id);
            playerHealths.erase(id);
            bouncingShapes.erase(id);
            activeStates.erase(id);
            return true;
        }
        return false;
    }), entities.end());
}


// RenderSystem implementation
void RenderSystem::update(sf::RenderWindow& window, const std::vector<EntityId>& entities, const ComponentMap<PositionComponent>& positions, const ComponentMap<ShapeComponent>& shapes) {
    for (EntityId id : entities) {
        if (positions.count(id) && shapes.count(id)) {
            const auto& pos = positions.at(id);
            const auto& shape = shapes.at(id);
            
            if (shape.shapeType == ShapeType::Circle) {
                sf::CircleShape circle(shape.size.x / 2.0f);
                circle.setFillColor(shape.color);
                circle.setOrigin(shape.size / 2.0f);
                circle.setPosition(pos.position);
                window.draw(circle);
            } else if (shape.shapeType == ShapeType::Triangle) {
                sf::CircleShape triangle(shape.size.x / 2.0f, 3);
                triangle.setFillColor(shape.color);
                triangle.setOrigin(shape.size / 2.0f);
                triangle.setPosition(pos.position);
                window.draw(triangle);
            } else if (shape.shapeType == ShapeType::Square) {
                sf::RectangleShape square(shape.size);
                square.setFillColor(shape.color);
                square.setOrigin(shape.size / 2.0f);
                square.setPosition(pos.position);
                window.draw(square);
            } else if (shape.shapeType == ShapeType::Hexagon) {
                sf::CircleShape hexagon(shape.size.x / 2.0f, 6);
                hexagon.setFillColor(shape.color);
                hexagon.setOrigin(shape.size / 2.0f);
                hexagon.setPosition(pos.position);
                window.draw(hexagon);
            }
        }
    }
}

void RenderSystem::renderHealthBar(sf::RenderWindow& window, sf::Vector2f position, float currentHealth, float maxHealth) {
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

void RenderSystem::renderShieldBar(sf::RenderWindow& window, sf::Vector2f position, float currentShield, float maxShield) {
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

// SoundSystem implementation
SoundSystem::SoundSystem() {
    if (!laserBuffer.loadFromFile("assets/laser.wav")) {
        std::cerr << "Error loading laser.wav" << std::endl;
    }
    if (!explosionBuffer.loadFromFile("assets/explosion.wav")) {
        std::cerr << "Error loading explosion.wav" << std::endl;
    }
    if (!shieldBuffer.loadFromFile("assets/shields.wav")) {
        std::cerr << "Error loading shields.wav" << std::endl;
    }
    if (!playerHitBuffer.loadFromFile("assets/explosion.wav")) {
        std::cerr << "Error loading explosion.wav" << std::endl;
    }
}

void SoundSystem::update(ComponentMap<SoundComponent>& sounds) {
    for (auto& pair : sounds) {
       if (pair.second.type == SoundType::PlayerShot) {
            activeSounds.emplace_back(laserBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundType::EnemyHit) {
            activeSounds.emplace_back(explosionBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundType::ShieldHit) {
            activeSounds.emplace_back(shieldBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundType::PlayerHit) {
            activeSounds.emplace_back(playerHitBuffer);
            activeSounds.back().play();
        }
    }
    sounds.clear();

    for (auto it = activeSounds.begin(); it != activeSounds.end(); ) {
        if (it->getStatus() == sf::SoundSource::Status::Stopped) {
            it = activeSounds.erase(it);
        } else {
            ++it;
        }
    }
}

// DebugSystem implementation
void DebugSystem::handleEvent(const sf::Event& event, bool& showImGui, GameState& gameState) {
    if (gameState != GameState::Running && gameState != GameState::Paused) {
        return;
    }
    
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::Tab) {
            showImGui = !showImGui;
        }
    }
}
