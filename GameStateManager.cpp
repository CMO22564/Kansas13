#include "GameStateManager.hpp"
#include <iostream>
#include <variant>
#include "LevelData.hpp" // Include the new header file
#include "LevelLoader.cpp" // Include the loader implementation

GameStateManager* GameStateManager::s_instance = nullptr;

GameStateManager& GameStateManager::getInstance() {
    if (!s_instance) {
        s_instance = new GameStateManager();
    }
    return *s_instance;
}

GameStateManager::GameStateManager() : m_currentState(GameState::TitleScreen), m_score(0) {}

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
    if (newState == GameState::Running && m_currentState != GameState::Running) {
        std::cout << "Transitioning to Running state. Resetting game..." << std::endl;
        m_score = 0;

        // ⭐ New: Load all levels at the start of the game
        if (m_levels.empty()) {
            m_levels = loadLevelsFromFile("level_data.json");
        }
        
        // Pass the first level's data to your systems
        LevelData currentLevel = m_levels[m_currentLevelIndex];
        // Now pass this `currentLevel` data to your EnemySpawnSystem
        // enemySpawnSystem.setParameters(currentLevel.enemyCount, currentLevel.spawnInterval);
    }
    m_currentState = newState;
    std::cout << "Game state changed to: " << static_cast<int>(m_currentState) << std::endl;
}

// ⭐ New: Add a function to advance to the next level
void GameStateManager::advanceToNextLevel() {
    if (m_currentLevelIndex + 1 < m_levels.size()) {
        m_currentLevelIndex++;
        setState(GameState::Running);
    } else {
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
