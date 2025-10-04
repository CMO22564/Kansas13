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
	ComponentMap<DamageComponent> damageValues;
	ComponentMap<ActiveComponent> activeStates;
	ComponentMap<PlayerHealthComponent> playerHealths;
	ComponentMap<HealthComponent> healths;
	ComponentMap<ShieldComponent> shields;
	ComponentMap<SoundComponent> sounds;
	ComponentMap<VelocityComponent> velocities;
	ComponentMap<EnemyComponent> enemies; // <-- ADD THIS LINE
	ComponentMap<PlayerInputComponent> playerInputs; // <-- Add this
	ComponentMap<PlayerLivesComponent> playerLives; // <-- Add this
	
	//ComponentMap<HealthComponent> healths;

    std::vector<EntityId> entities;

    // Systems
    PlayerInputSystem playerInputSystem;
    // EnemySpawnSystem enemySpawnSystem; // The system is now accessed via singleton
    MovementSystem movementSystem;
    CombatSystem combatSystem;
    CleanUpSystem cleanUpSystem;
    RenderSystem renderSystem;
    SoundSystem soundSystem;
    DebugSystem debugSystem;
    ScreenSystem screenSystem(window, "arial.ttf");

    EntityId playerId = getNextEntityId();
    entities.push_back(playerId);
    
    // int score = 0; // REMOVED: Score is now managed by GameStateManager
     
    // Add player components
    positions.emplace(playerId, PositionComponent{{sf::Vector2f(400, 500)}}); 
    velocities.emplace(playerId, VelocityComponent{{sf::Vector2f(0.f, 0.f)}});
    playerInputs.emplace(playerId, PlayerInputComponent{});
    playerHealths.emplace(playerId, PlayerHealthComponent{100.0f, 100.0f}); // This is now correct with double braces
shields.emplace(playerId, ShieldComponent{100.0f, 100.0f});             // This is now correct with double braces
playerLives.emplace(playerId, PlayerLivesComponent{3});                  // This is now correct with double braces

    // FIX: Change initialization to the required explicit style
    RenderComponent playerShape;
    playerShape.type = RenderComponent::Type::Square; 
    playerShape.color = sf::Color::Green;
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
            
            GameStateManager::getInstance().handleInput(*event);
        }

        sf::Time deltaTime = gameClock.restart();
    // Assuming you have a variable named 'dt' which stores the frame time.
    // If you don't have one, you need to create one (e.g., float dt = clock.restart().asSeconds();)
        float dt = gameClock.restart().asSeconds(); // <--- CRITICAL: Ensure you have 'dt' defined!


        ImGui::SFML::Update(window, deltaTime);

        if (GameStateManager::getInstance().getState() == GameState::Running) {
            
            playerInputSystem.update(entities, 
                         dt, // <--- FIX: Insert the missing 'dt' argument here
                         positions, 
                         velocities, 
                         playerInputs, 
                         entities, // Note: You're passing 'entities' twice, once as list of entities, once as the list to add new projectiles to.
                         shapes, 
                         projectiles, 
                         activeStates, 
                         sounds, 
                         damageValues);
            
            // Accessing EnemySpawnSystem as a singleton
            EnemySpawnSystem::getInstance().update(entities, positions, velocities, shapes, bouncingShapes, activeStates, damageValues, healths, enemies);
            
            movementSystem.update(entities, positions, velocities, bouncingShapes, shapes, deltaTime);
                 
            // CombatSystem signature is updated, no score parameter
            combatSystem.update(entities, positions, shapes, projectiles, bouncingShapes, damages, activeStates, playerHealths, healths, shields, sounds, velocities, enemies);
            
            soundSystem.update(sounds);
            
            // Normal cleanup runs every frame
            cleanUpSystem.update(entities, activeStates, projectiles, bouncingShapes, damageValues, playerHealths, shields, playerLives, playerInputs, positions, velocities, shapes, sounds);
            
            // CRITICAL NEW LOGIC: Level completion check and advancement
            if (EnemySpawnSystem::getInstance().isLevelComplete()) {
                std::cout << "Main loop detected level complete. Advancing level..." << std::endl;
                
                // Force-clear all enemies and projectiles BEFORE advancing the state
                cleanUpSystem.clearNonPlayerEntities(
                    entities, 
                    projectiles, 
                    bouncingShapes, 
                    damageValues, 
                    activeStates, 
                    positions, 
                    velocities, 
                    shapes, 
                    sounds
                );
                
                GameStateManager::getInstance().advanceToNextLevel();
            }
        }

        // Player clamping code (unchanged, but using std::max/min)
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

        if (GameStateManager::getInstance().getState() == GameState::Running) {
            renderSystem.update(window, entities, positions, shapes, activeStates);
        } 	

        screenSystem.update(GameStateManager::getInstance().getState(), GameStateManager::getInstance().getScore(), playerHealths, shields, playerLives);
        
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
