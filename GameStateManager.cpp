#include "GameStateManager.hpp"
#include <iostream>
#include <variant>

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
    }
    m_currentState = newState;
    std::cout << "Game state changed to: " << static_cast<int>(m_currentState) << std::endl;
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
