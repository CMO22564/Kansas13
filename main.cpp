#include "imconfig.h"
#include <cstring>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Core.hpp"
#include "PlayerInputSystem.hpp"
#include "MovementSystem.hpp"
#include "CombatSystem.hpp"
#include "CleanUpSystem.hpp"
#include "RenderSystem.hpp"
#include "SoundSystem.hpp"
#include "DebugSystem.hpp"
#include "ScreenSystem.hpp"
#include "GameStateManager.hpp"
#include "EnemySpawnSystem.hpp"
#include <iostream>
#include <algorithm> // For std::max/std::min

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Kansas 13");
    window.setFramerateLimit(60);

// Define the font path (adjust to your actual font file location)
    std::string fontPath = "arial.ttf"; // Replace with actual path, e.g., "assets/fonts/arial.ttf"

    // Instantiate ScreenSystem with window and font path
    // ScreenSystem screenSystem(window, fontPath);

    ImGui::SFML::Init(window);
    sf::Clock imguiClock;

    // Debug window flag
    bool showDebugWindow = false;

    // --- Component Maps ---
    ComponentMap<PositionComponent> positions;
    ComponentMap<RenderComponent> shapes;
    ComponentMap<ProjectileComponent> projectiles;
    ComponentMap<BouncingComponent> bouncingShapes;
    ComponentMap<DamageComponent> damages;
    ComponentMap<ActiveComponent> activeStates;
    ComponentMap<PlayerHealthComponent> playerHealths;
    ComponentMap<HealthComponent> healths;
    ComponentMap<ShieldComponent> shields;
    ComponentMap<SoundComponent> sounds;
    ComponentMap<VelocityComponent> velocities;
    ComponentMap<EnemyComponent> enemies;
    ComponentMap<PlayerInputComponent> playerInputs;
    ComponentMap<PlayerLivesComponent> playerLives;

    std::vector<EntityId> entities;

    // Systems
    PlayerInputSystem playerInputSystem;
    MovementSystem movementSystem;
    CombatSystem combatSystem;
    CleanUpSystem cleanUpSystem;
    RenderSystem renderSystem;
    SoundSystem soundSystem;
    DebugSystem debugSystem;
    ScreenSystem screenSystem(window, "arial.ttf"); 

    EntityId playerId = getNextEntityId();
    entities.push_back(playerId);
    
    // Add player components
    positions.emplace(playerId, PositionComponent{{sf::Vector2f(400, 500)}}); 
    velocities.emplace(playerId, VelocityComponent{{sf::Vector2f(0.f, 0.f)}});
    playerInputs.emplace(playerId, PlayerInputComponent{});
    playerHealths.emplace(playerId, PlayerHealthComponent{100.0f, 100.0f});
    shields.emplace(playerId, ShieldComponent{100.0f, 100.0f});
    playerLives.emplace(playerId, PlayerLivesComponent{3});

    // FIX: Change initialization to the required explicit style
    RenderComponent playerShape;
    playerShape.type = RenderComponent::Type::Triangle; 
    playerShape.color = sf::Color::Magenta;
    playerShape.size = 25.f; 

    // Emplace the explicitly initialized object
    activeStates.emplace(playerId, ActiveComponent{true});
    shapes.emplace(playerId, std::move(playerShape));
    
    sf::Clock gameClock;

    // Game loop
    while (window.isOpen()) {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Tab key to toggle on and off the Debug Window.
            if (event->is<sf::Event::KeyPressed>()) {
                const auto* keyEvent = event->getIf<sf::Event::KeyPressed>();
                if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    showDebugWindow = !showDebugWindow;
                }
            }
            
            // FIXED: Handle ScoreEntry state separately
            if (GameStateManager::getInstance().getState() == GameState::ScoreEntry) {
                playerInputSystem.handleScoreEntry(*event, GameStateManager::getInstance());
            } else {
                GameStateManager::getInstance().handleInput(*event);
            }
        }

        sf::Time deltaTime = gameClock.restart();
        float dt = deltaTime.asSeconds();

        ImGui::SFML::Update(window, deltaTime);

        if (GameStateManager::getInstance().getState() == GameState::Running) {
            
            playerInputSystem.update(entities, 
                         dt,
                         positions, 
                         velocities, 
                         playerInputs, 
                         entities,
                         shapes, 
                         projectiles, 
                         activeStates, 
                         sounds, 
                         damages);
            
            // Accessing EnemySpawnSystem as a singleton
            EnemySpawnSystem::getInstance().update(entities, positions, velocities, shapes, bouncingShapes, activeStates, damages, healths, enemies);
            
            movementSystem.update(entities, positions, velocities, bouncingShapes, shapes, deltaTime);
                 
            // CombatSystem signature is updated, no score parameter
            combatSystem.update(entities, positions, shapes, projectiles, bouncingShapes, damages, activeStates, playerHealths, healths, shields, sounds, velocities, enemies, playerLives);
            
            soundSystem.update(sounds);
            
            // Normal cleanup runs every frame
            cleanUpSystem.update(entities, activeStates, projectiles, bouncingShapes, damages, playerHealths, shields, playerLives, playerInputs, positions, velocities, shapes, sounds);
            
            // CRITICAL NEW LOGIC: Level completion check and advancement
            if (EnemySpawnSystem::getInstance().isLevelComplete()) {
                std::cout << "Main loop detected level complete. Advancing level..." << std::endl;
                
                // Force-clear all enemies and projectiles BEFORE advancing the state
                cleanUpSystem.clearNonPlayerEntities(
                    entities, 
                    projectiles, 
                    bouncingShapes, 
                    damages,
                    activeStates, 
                    positions, 
                    velocities, 
                    shapes, 
                    sounds
                );
                
                GameStateManager::getInstance().advanceToNextLevel();
            }

                // 🛑 CRITICAL FIX: Call the ScreenSystem update function here
                screenSystem.update(
                GameStateManager::getInstance().getState(),
                GameStateManager::getInstance().getScore(),
                playerHealths,
                shields,
                playerLives
            );


        }

        // Player clamping code
        const float windowWidth = 800.0f;
        const float windowHeight = 600.0f;
        const float playerRadius = 60.0f;
        const float leftBound = playerRadius;
        const float rightBound = windowWidth - playerRadius;
        const float topBound = playerRadius;
        const float bottomBound = windowHeight - playerRadius;

        if (positions.count(playerId)) {
            auto& playerPos = positions.at(playerId).position;
            playerPos.x = std::max(leftBound, std::min(playerPos.x, rightBound));
            playerPos.y = std::max(topBound, std::min(playerPos.y, bottomBound));
        }

        // Debug system update
        if (showDebugWindow) {
            debugSystem.update(GameStateManager::getInstance().getScore(), playerHealths, shields, playerLives, entities, positions, shapes, velocities);
        }

       window.clear();

        // FIXED: Always render the game entities if not on title screen
        if (GameStateManager::getInstance().getState() != GameState::TitleScreen) { // <-- FIXED: getCurrentState -> getState
            renderSystem.update(window, entities, positions, shapes, activeStates);
        }

        // FIXED: Call render() instead of update()
        screenSystem.render(window, GameStateManager::getInstance());
        
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}