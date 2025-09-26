#include "GameStateManager.hpp"
#include <iostream>
#include <variant>
#include "LevelData.hpp"
#include "LevelLoader.hpp"
#include "EnemySpawnSystem.hpp"
#include "Core.hpp"


GameStateManager* GameStateManager::s_instance = nullptr;

GameStateManager& GameStateManager::getInstance() {
    if (!s_instance) {
        s_instance = new GameStateManager();
    }
    return *s_instance;
}

GameStateManager::GameStateManager() : m_currentState(GameState::TitleScreen), m_score(0), m_currentLevelIndex(0) {}

void GameStateManager::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (m_currentState == GameState::TitleScreen && keyEvent->code == sf::Keyboard::Key::P) {
            setState(GameState::Running);
        } else if (m_currentState == GameState::Running && keyEvent->code == sf::Keyboard::Key::Escape) {
            setState(GameState::Paused);
        } else if (m_currentState == GameState::Paused && keyEvent->code == sf::Keyboard::Key::Escape) {
            setState(GameState::Running);
        }
    }
}

void GameStateManager::setState(GameState newState) {
    
    // ⭐ FIX 1: Change the condition to ONLY check if the new state is Running.
    // REMOVE '&& m_currentState != GameState::Running'
    if (newState == GameState::Running) { 
        // GameStateManager.cpp - inside setState(GameState::Running)

// 1. Initial Load: Load ALL levels only once
if (m_levels.empty()) {
    std::cout << "DEBUG 1: Before calling loadLevelsFromFile..." << std::endl; // <-- TRACE POINT 1
    m_levels = loadLevelsFromFile("level_data.json"); 
    
    if (m_levels.empty()) {
        std::cerr << "CRITICAL ERROR: Failed to load any levels." << std::endl;
        m_currentState = GameState::GameOver;
        return;
    }
    std::cout << "DEBUG 2: Level loading complete." << std::endl; // <-- TRACE POINT 2
    // ...
}

// 2. Access the data
std::cout << "DEBUG 3: Accessing currentLevel data..." << std::endl; // <-- TRACE POINT 3
const LevelData& currentLevel = m_levels[m_currentLevelIndex];

// --- NEW CODE: Extract the first enemy type ---
        // Assuming every level has at least one enemy type defined (index 0)
        if (currentLevel.enemyTypes.empty()) {
             std::cerr << "CRITICAL ERROR: No enemy types defined for Level " 
                       << currentLevel.levelNumber << std::endl;
             // Handle error or use a default type
             m_currentState = GameState::GameOver;
             return;
        }
        const EnemyTypeData& enemyTypeData = currentLevel.enemyTypes[0];

// 3. Call the Singleton
std::cout << "DEBUG 4: Calling EnemySpawnSystem::getInstance()..." << std::endl; // <-- TRACE POINT 4
EnemySpawnSystem::getInstance().setLevelParameters( 
    currentLevel.enemyCount, 
    currentLevel.spawnInterval,
    currentLevel.minX,
    currentLevel.maxX
);

// Pass the new EnemyTypeData parameters!
        // NOTE: setLevelParameters in EnemySpawnSystem.hpp must be updated next!
        EnemySpawnSystem::getInstance().setLevelParameters( 
            currentLevel.enemyCount, 
            currentLevel.spawnInterval,
            currentLevel.minX,
            currentLevel.maxX,
            // --- PASS THE NEW DATA HERE ---
            enemyTypeData 
        );

std::cout << "DEBUG 5: Singleton call complete. Level ready." << std::endl; // <-- TRACE POINT 5

    }
    
    m_currentState = newState;
    std::cout << "Game state changed to: " << static_cast<int>(m_currentState) << std::endl;
}

void GameStateManager::advanceToNextLevel() {
    if (m_currentLevelIndex + 1 < m_levels.size()) {
        m_currentLevelIndex++; // Increment to the next index
        std::cout << "Advancing to Level " << m_levels[m_currentLevelIndex].levelNumber << std::endl;
        
        // ⭐ FIX 2: Simply call setState(Running). It will now correctly trigger setup.
        setState(GameState::Running); 
    } else {
        // No more levels defined
        std::cout << "Congratulations! You've completed the game." << std::endl;
        setState(GameState::GameOver);
    }
}

GameState GameStateManager::getState() const {
    return m_currentState;
}

int GameStateManager::getScore() const {
    return m_score;
}

void GameStateManager::setScore(int newScore) {
    m_score = newScore;
}


// Utility function to convert JSON string to RenderComponent::Type
RenderComponent::Type shapeTypeFromString(const std::string& typeStr) {
    if (typeStr == "Triangle") {
        return RenderComponent::Triangle;
    }
    if (typeStr == "Square") {
        return RenderComponent::Square;
    }
    // Default to Circle if not recognized (matches Level 1 default)
    return RenderComponent::Circle;
}

    // Utility function to convert JSON string color to sf::Color
    sf::Color colorFromString(const std::string& colorStr) {
    if (colorStr == "Red") {
        return sf::Color::Red;
    }
    if (colorStr == "Blue") {
        return sf::Color::Blue;
    }
    // Add other colors as needed...
    // Default to white for safety
    return sf::Color::White;
}
