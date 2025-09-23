#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Core.hpp"
#include "GameStateManager.hpp"

class ScreenSystem {
public:
    ScreenSystem(sf::RenderWindow& window, const std::string& fontPath);
    void update(const GameState& gameState, const int& score,
                const ComponentMap<PlayerHealthComponent>& playerHealths,
                const ComponentMap<ShieldComponent>& shields,
                const ComponentMap<PlayerLivesComponent>& playerLives);

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_titleText;
    sf::Text m_playText;
    sf::Text m_pauseText;
    sf::Text m_controlsText;
    sf::Text m_scoreText;
    sf::Text m_levelText;
    sf::Text m_livesText;
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBar;
    sf::RectangleShape m_shieldBarBackground;
    sf::RectangleShape m_shieldBar;
};
