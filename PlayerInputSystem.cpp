#include "PlayerInputSystem.hpp"
#include <cmath>
#include <limits>
#include <iostream>

void PlayerInputSystem::update(
    const std::vector<EntityId>& entities,
    const ComponentMap<EnemyComponent>& enemyComponents, // Move to 2nd position
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
    if (gsm.getState() != GameState::Running) return;

    for (EntityId entityId : entities) {
        if (!positions.count(entityId) || !playerInputs.count(entityId)) continue;

        auto& posComp = positions.at(entityId);
        auto& input = playerInputs.at(entityId);
        sf::Vector2f playerPos = posComp.position;

        // 1. Movement
        sf::Vector2f moveDir(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveDir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveDir.x += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) moveDir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveDir.y += 1.f;

        if (velocities.count(entityId)) {
            velocities.at(entityId).velocity = moveDir * 400.0f; // 🌟 Fix
        }

        // 2. Shooting
        input.shootCooldown += dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && input.shootCooldown >= input.shootInterval) {
            sf::Vector2f aimDir(0.f, -1.f); 
            float bulletSpeed = 600.0f;

            if (input.turretLevel > 0) {
                EntityId targetId = findNearestEnemy(playerPos, entitiesList, positions, activeStates, enemyComponents);
                if (targetId != -1) {
                    sf::Vector2f enemyPos = positions.at(targetId).position;
                    if (input.turretLevel == 1) {
                        aimDir = enemyPos - playerPos;
                    } else if (input.turretLevel == 2) {
                        sf::Vector2f enemyVel = velocities.count(targetId) ? velocities.at(targetId).velocity : sf::Vector2f(0,0);
                        float dist = std::sqrt(std::pow(enemyPos.x - playerPos.x, 2) + std::pow(enemyPos.y - playerPos.y, 2));
                        aimDir = (enemyPos + (enemyVel * (dist / bulletSpeed))) - playerPos;
                    }
                }
            }

            // Spawn Bullet
            float mag = std::sqrt(aimDir.x * aimDir.x + aimDir.y * aimDir.y);
            sf::Vector2f bulletVel = (mag > 0) ? (aimDir / mag) * bulletSpeed : sf::Vector2f(0, -bulletSpeed);

            EntityId bId = getNextEntityId();
            entitiesList.push_back(bId);
            positions[bId] = { playerPos };
            velocities[bId] = { bulletVel };
            activeStates[bId] = { true };
            projectiles[bId] = {};
            damages[bId] = { 15.0f };
            
            RenderComponent r; r.type = RenderComponent::Circle; r.size = 4.0f; r.color = sf::Color::Yellow;
            shapes[bId] = std::move(r);
            sounds[bId] = { SoundComponent::Type::Laser };
            
            input.shootCooldown = 0.0f;
        }
    }
}

void PlayerInputSystem::handleScoreEntry(const sf::Event& event, GameStateManager& manager) {
    // 🌟 This line below is for SFML 3 compatibility
    auto const* textEvent = event.getIf<sf::Event::TextEntered>();
    if (!textEvent) return;
    
    uint32_t unicode = textEvent->unicode;
    
    // Initial Entry Logic (A-Z)
    if (unicode < 128) { 
        char c = static_cast<char>(unicode);
        if (m_currentInitialIndex < 3 && std::isalpha(c)) {
            m_currentInitials[m_currentInitialIndex] = std::toupper(c);
            m_currentInitialIndex++;
            manager.setCurrentInitials(m_currentInitials);
        }
    }

    // Backspace (unicode 8)
    if (unicode == 8 && m_currentInitialIndex > 0) {
        m_currentInitialIndex--;
        m_currentInitials[m_currentInitialIndex] = '-';
        manager.setCurrentInitials(m_currentInitials);
    }

    // Enter (unicode 13) - Require exactly 3 initials
    if (unicode == 13 && m_currentInitialIndex == 3) {
        manager.getHighScoreManager().addScore(m_currentInitials.substr(0, 3), manager.getScore());
        m_currentInitials = "---";
        m_currentInitialIndex = 0;
        manager.setCurrentInitials(m_currentInitials);
        
        // Reset game state and return to Title
        manager.setScore(0);
        manager.setCurrentLevelIndex(0);
        manager.setState(GameState::TitleScreen);
    }
}

EntityId PlayerInputSystem::findNearestEnemy(sf::Vector2f playerPos, const std::vector<EntityId>& entitiesList, 
    const ComponentMap<PositionComponent>& positions, const ComponentMap<ActiveComponent>& activeStates, 
    const ComponentMap<EnemyComponent>& enemyComponents) {
    
    EntityId closest = -1;
    float minDist = std::numeric_limits<float>::max();
    for (auto const& [id, enemy] : enemyComponents) {
        if (activeStates.count(id) && activeStates.at(id).active) {
            float d = std::sqrt(std::pow(positions.at(id).position.x - playerPos.x, 2) + std::pow(positions.at(id).position.y - playerPos.y, 2));
            if (d < minDist) { minDist = d; closest = id; }
        }
    }
    return closest;
}