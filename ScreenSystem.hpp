#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Core.hpp"
#include "GameStateManager.hpp"

class ScreenSystem {
public:
    ScreenSystem(sf::RenderWindow& window, const std::string& fontPath);
    void render(sf::RenderWindow& window, GameStateManager& manager);
    void update(const GameState& gameState, const int& score, const int& levelIndex,
                const ComponentMap<PlayerHealthComponent>& playerHealths,
                const ComponentMap<ShieldComponent>& shields,
                const ComponentMap<PlayerLivesComponent>& playerLives,
                const ComponentMap<PlayerInputComponent>& playerInputs);
private:
    sf::RenderWindow& m_window;
    sf::Font m_font; // KEEP: Only the font needs to be a persistent member

    sf::Text m_titleText;
    sf::Text m_playText;
    sf::Text m_pauseText;
    sf::Text m_controlsText;
    sf::Text m_scoreText;
    sf::Text m_levelText;
    sf::Text m_livesText;
    sf::Text m_creditsText; // ADDED: For displaying currency
    sf::Text m_gameOverText; 

    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBar;
    sf::RectangleShape m_shieldBarBackground;
    sf::RectangleShape m_shieldBar;

// ADD THESE MISSING PRIVATE DECLARATIONS:
    void renderTitleScreen(sf::RenderWindow& window, GameStateManager& manager);
    void renderPauseScreen(sf::RenderWindow& window);
    void renderGameOverScreen(sf::RenderWindow& window, GameStateManager& manager);
    void renderScoreEntryScreen(sf::RenderWindow& window, GameStateManager& manager);
    void renderLevelTransitionScreen(sf::RenderWindow& window, GameStateManager& manager);
};
