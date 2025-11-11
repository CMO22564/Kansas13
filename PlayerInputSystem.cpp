#include "PlayerInputSystem.hpp"
#include <SFML/Window/Event.hpp>
#include "Core.hpp"
#include "GameStateManager.hpp"
#include <iostream>
#include <cmath>
#include <limits>

void PlayerInputSystem::update(
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
    ComponentMap<DamageComponent>& damages) {
    
    GameStateManager& gsm = GameStateManager::getInstance();
    GameState state = gsm.getState();

    if (state != GameState::Running) {
        return; // Only handle player movement and shooting in Running state
    }

    for (EntityId entityId : entities) {
        if (!positions.count(entityId) || !velocities.count(entityId) || !playerInputs.count(entityId)) {
            continue; // Skip entities without required components
        }

        auto& velocity = velocities.at(entityId).velocity;
        auto& input = playerInputs.at(entityId);

        // Player movement logic
        sf::Vector2f moveDirection(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            moveDirection.x -= 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            moveDirection.x += 1.0f;
        }

        float speed = 500.0f;
        velocity = moveDirection * speed;

        // Projectile shooting logic
        const float SHOOT_COOLDOWN = 0.1f; // Balanced cooldown for controlled firing
        input.shootCooldown += dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && input.shootCooldown >= SHOOT_COOLDOWN) {
            sf::Vector2f playerPos = positions.at(entityId).position;

            // Find the closest enemy to target
            EntityId closestEnemy = 0;
            float minDistance = std::numeric_limits<float>::max();

            for (EntityId currentEntity : entitiesList) {
                if (currentEntity == entityId || !activeStates.count(currentEntity) || !activeStates.at(currentEntity).active) {
                    continue;
                }
                if (positions.count(currentEntity) && shapes.count(currentEntity)) {
                    sf::Vector2f enemyPos = positions.at(currentEntity).position;
                    float dist = std::sqrt(std::pow(playerPos.x - enemyPos.x, 2) + std::pow(playerPos.y - enemyPos.y, 2));
                    if (dist < minDistance) {
                        minDistance = dist;
                        closestEnemy = currentEntity;
                    }
                }
            }

            // Create projectile
            EntityId projectileId = getNextEntityId();
            entitiesList.push_back(projectileId);
            positions.emplace(projectileId, PositionComponent{playerPos});

            RenderComponent projectileRender;
            projectileRender.type = RenderComponent::Type::Circle;
            projectileRender.color = sf::Color::Yellow;
            projectileRender.size = 5.0f;

            if (closestEnemy != 0) {
                // Shoot toward closest enemy
                sf::Vector2f enemyPos = positions.at(closestEnemy).position;
                sf::Vector2f direction = enemyPos - playerPos;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length > 0) {
                    sf::Vector2f normalizedDirection = direction / length;
                    float projectileSpeed = 800.0f;
                    velocities.emplace(projectileId, VelocityComponent{normalizedDirection * projectileSpeed});
                } else {
                    velocities.emplace(projectileId, VelocityComponent{sf::Vector2f(0.f, -800.0f)});
                }
                input.shootCooldown = 0.0f; // Faster reset for homing shots
            } else {
                // Fallback: shoot upward
                velocities.emplace(projectileId, VelocityComponent{sf::Vector2f(0.f, -800.0f)});
                input.shootCooldown = 0.05f; // Slightly slower for fallback
            }

            shapes.emplace(projectileId, std::move(projectileRender));
            projectiles.emplace(projectileId, ProjectileComponent{});
            activeStates.emplace(projectileId, ActiveComponent{true});
            sounds.emplace(projectileId, SoundComponent{SoundComponent::Type::Laser});
            damages.emplace(projectileId, DamageComponent{10.0f});
        }
    }
}

void PlayerInputSystem::handleScoreEntry(sf::Event& event, GameStateManager& manager) {
    if (!event.is<sf::Event::TextEntered>()) {
        return; // Only handle TextEntered events
    }

    const auto* textEvent = event.getIf<sf::Event::TextEntered>();
    if (!textEvent) return;
    
    uint32_t unicode = textEvent->unicode;
    
    if (unicode < 128) { // Restrict to ASCII for initials
        char c = static_cast<char>(unicode);
        if (m_currentInitialIndex < 3 && std::isalpha(c)) {
            m_currentInitials[m_currentInitialIndex] = std::toupper(c);
            m_currentInitialIndex++;
            // UPDATE: Sync with manager so screen can display it
            manager.setCurrentInitials(m_currentInitials);
        }
    }

    // Backspace (unicode 8)
    if (unicode == 8 && m_currentInitialIndex > 0) {
        m_currentInitialIndex--;
        m_currentInitials[m_currentInitialIndex] = '-';
        // UPDATE: Sync with manager so screen can display it
        manager.setCurrentInitials(m_currentInitials);
    }

    // Enter (unicode 13) - Require exactly 3 initials
    if (unicode == 13 && m_currentInitialIndex == 3) {
        manager.getHighScoreManager().addScore(m_currentInitials.substr(0, 3), manager.getScore());
        m_currentInitials = "---";
        m_currentInitialIndex = 0;
        m_enteringScore = false;
        // UPDATE: Sync with manager
        manager.setCurrentInitials(m_currentInitials);
        manager.setState(GameState::GameOver);
    }
}

void PlayerInputSystem::resetInitials() {
    m_currentInitials = "---";
    m_currentInitialIndex = 0;
    m_enteringScore = true;
}