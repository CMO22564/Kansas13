#pragma once
#include <SFML/Window/Event.hpp>
#include <vector>
#include "Core.hpp"
#include "LevelData.hpp"
#include "HighScore.hpp"

enum class GameState {
    TitleScreen,
    Running,
    Paused,
    GameOver,
    LevelComplete,
    ScoreEntry // Added for high score entry
};

class GameStateManager {
public:
    static GameStateManager& getInstance() {
        static GameStateManager instance; // Simplified singleton
        return instance;
    }
    void handleInput(const sf::Event& event);
    GameState getState() const;
    void setState(GameState newState);
    int getScore() const;
    void setScore(int newScore);
    void setCurrentLevelIndex(int index) { m_currentLevelIndex = index; } // <-- REQUIRED public method
    void advanceToNextLevel();
    int getCurrentLevel() const;
    HighScoreManager& getHighScoreManager() { return m_highScoreManager; } // Added for high score
    void setCurrentInitials(const std::string& initials);
    const std::string& getCurrentInitials() const;
    int getCurrentLevelIndex() const;

private:
    GameStateManager() : m_currentState(GameState::TitleScreen), m_score(0), m_currentLevelIndex(0) {}
    static GameStateManager* s_instance; // Still needed for compatibility
    GameState m_currentState;
    int m_score;
    std::vector<LevelData> m_levels;
    int m_currentLevelIndex;
    HighScoreManager m_highScoreManager; // Added for high score
    // CRITICAL: Ensure this member is declared for score entry
    std::string m_currentInitials = "---";
};