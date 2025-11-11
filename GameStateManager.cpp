#include "GameStateManager.hpp"
#include <iostream>
#include "LevelData.hpp"
#include "LevelLoader.hpp"
#include "EnemySpawnSystem.hpp"
#include "Core.hpp"
#include "Utils.hpp"

GameStateManager* GameStateManager::s_instance = nullptr;

void GameStateManager::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (m_currentState == GameState::TitleScreen && keyEvent->code == sf::Keyboard::Key::P) {
            setState(GameState::Running);
        } else if (m_currentState == GameState::Running && keyEvent->code == sf::Keyboard::Key::Escape) {
            setState(GameState::Paused);
        } else if (m_currentState == GameState::Paused && keyEvent->code == sf::Keyboard::Key::Escape) {
            setState(GameState::Running);
        } else if (m_currentState == GameState::GameOver && keyEvent->code == sf::Keyboard::Key::P) {
            m_score = 0; // Reset score
            m_currentLevelIndex = 0; // Reset level
            setState(GameState::TitleScreen); // Return to title screen
        }
    }
}

void GameStateManager::setState(GameState newState) {
    if (newState == GameState::Running) {
        if (m_levels.empty()) {
            std::cout << "DEBUG 1: Before calling loadLevelsFromFile..." << std::endl;
            m_levels = loadLevelsFromFile("level_data.json");
            
            if (m_levels.empty()) {
                std::cerr << "CRITICAL ERROR: Failed to load any levels." << std::endl;
                m_currentState = GameState::GameOver;
                return;
            }
            std::cout << "DEBUG 2: Level loading complete." << std::endl;
        }

        std::cout << "DEBUG 3: Accessing currentLevel data..." << std::endl;
        const LevelData& currentLevel = m_levels[m_currentLevelIndex];

        if (currentLevel.enemyTypes.empty()) {
            std::cerr << "CRITICAL ERROR: No enemy types defined for Level "
                      << currentLevel.levelNumber << std::endl;
            m_currentState = GameState::GameOver;
            return;
        }

        std::cout << "DEBUG 4: Calling EnemySpawnSystem::getInstance()..." << std::endl;
        EnemySpawnSystem::getInstance().setLevelParameters(
            currentLevel.enemyCount,
            currentLevel.spawnInterval,
            currentLevel.minX,
            currentLevel.maxX,
            currentLevel.enemyTypes[0]
        );

        std::cout << "DEBUG 5: Singleton call complete. Level "
                  << currentLevel.levelNumber << " ready." << std::endl;
    } else if (newState == GameState::ScoreEntry) {
        std::cout << "Entering score entry for score: " << m_score << std::endl;
    }

    m_currentState = newState;
    std::cout << "Game state changed to: " << static_cast<int>(m_currentState) << std::endl;
}

void GameStateManager::advanceToNextLevel() {
    if (m_currentLevelIndex + 1 < m_levels.size()) {
        m_currentLevelIndex++;
        std::cout << "Advancing to Level " << m_levels[m_currentLevelIndex].levelNumber << std::endl;
        setState(GameState::Running);
    } else {
        std::cout << "Congratulations! You've completed the game." << std::endl;
        if (m_highScoreManager.isHighScore(m_score)) {
            setState(GameState::ScoreEntry);
        } else {
            setState(GameState::GameOver);
        }
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

// GameStateManager.cpp

// ... (existing function definitions, e.g., setState, advanceToNextLevel, etc.)

// REQUIRED DEFINITIONS TO FIX LINKER ERRORS

/**
 * @brief Sets the initials string (used during score entry).
 * Required by PlayerInputSystem.o
 */
void GameStateManager::setCurrentInitials(const std::string& initials) {
    m_currentInitials = initials;
}

/**
 * @brief Gets the current initials string.
 * Required by ScreenSystem.o
 */
const std::string& GameStateManager::getCurrentInitials() const {
    return m_currentInitials;
}

/**
 * @brief Gets the index of the current level.
 * Required by ScreenSystem.o
 */
int GameStateManager::getCurrentLevelIndex() const {
    return m_currentLevelIndex;
}


int GameStateManager::getCurrentLevel() const {
    if (m_currentLevelIndex < m_levels.size()) {
        return m_levels[m_currentLevelIndex].levelNumber;
    }
    return 1;
}