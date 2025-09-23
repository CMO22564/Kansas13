#pragma once
#include <SFML/Window/Event.hpp>
#include <variant>
#include "Core.hpp"

class GameStateManager {
public:
    static GameStateManager& getInstance();
    GameStateManager();
    void handleInput(const sf::Event& event);
    GameState getState() const;
    void setState(GameState newState);
    int getScore() const;
    void setScore(int newScore);

private:
    static GameStateManager* s_instance;
    GameState m_currentState;
    int m_score;
};
