#include "GameStateManager.hpp"
#include <iostream>
#include <variant>
#include "LevelData.hpp"
#include "LevelLoader.hpp"
#include "EnemySpawnSystem.hpp"

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
        std::cout << "Transitioning to Running state. Setting up level..." << std::endl;

        // ... (The rest of your existing level setup logic remains here) ...
        // 1. Initial Load: Load ALL levels only once
        // 2. Safety check against index overflow
        // 3. Get the data for the CURRENT level index
        
        // 4. Pass the level data to the EnemySpawnSystem
        const LevelData& currentLevel = m_levels[m_currentLevelIndex];
        EnemySpawnSystem::getInstance().setLevelParameters(
            currentLevel.enemyCount, 
            currentLevel.spawnInterval,
            currentLevel.minX,
            currentLevel.maxX
        );
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
