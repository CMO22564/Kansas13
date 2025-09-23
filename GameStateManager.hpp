#pragma once
#include <SFML/Window/Event.hpp>
#include <variant>
#include <vector> // You need this for std::vector
#include "Core.hpp"
#include "LevelData.hpp"

enum class GameState {
    TitleScreen,
    Running,
    Paused,
    GameOver,
    LevelComplete // You'll need this for later
};

class GameStateManager {
public:
    static GameStateManager& getInstance();
    GameStateManager();
    void handleInput(const sf::Event& event);
    GameState getState() const;
    void setState(GameState newState);
    int getScore() const;
    void setScore(int newScore);
    void advanceToNextLevel(); // ⭐ Declaration for the new function

private:
    static GameStateManager* s_instance;
    GameState m_currentState;
    int m_score;
    // LevelData m_currentLevelData; // Not needed if using a vector
    std::vector<LevelData> m_levels;
    int m_currentLevelIndex = 0;
};
