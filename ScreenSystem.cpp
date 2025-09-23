#include "ScreenSystem.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

ScreenSystem::ScreenSystem(sf::RenderWindow& window, const std::string& fontPath)
    : m_window(window),
      m_titleText(m_font, "CHAOS Kansas12B"),
      m_playText(m_font, "Press 'P' to Play"),
      m_pauseText(m_font, "PAUSED"),
      m_controlsText(m_font, "Use WASD to move. Press SPACE to shoot."),
      m_scoreText(m_font),
      m_levelText(m_font),
      m_livesText(m_font),
      m_healthBarBackground(),
      m_healthBar(),
      m_shieldBarBackground(),
      m_shieldBar()
{
    if (!m_font.openFromFile(fontPath)) { // Matches your SFML build
        std::cerr << "Failed to load font: " << fontPath << std::endl;
    }

    m_titleText.setFont(m_font);
    m_playText.setFont(m_font);
    m_pauseText.setFont(m_font);
    m_controlsText.setFont(m_font);
    m_scoreText.setFont(m_font);
    m_levelText.setFont(m_font);
    m_livesText.setFont(m_font);

    m_titleText.setCharacterSize(50);
    m_titleText.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(sf::Vector2f(titleBounds.position.x + titleBounds.size.x / 2.f,
                                       titleBounds.position.y + titleBounds.size.y / 2.f));
    m_titleText.setPosition(sf::Vector2f(m_window.getSize().x / 2.f, 100));

    m_playText.setCharacterSize(30);
    m_playText.setFillColor(sf::Color::White);
    sf::FloatRect playBounds = m_playText.getLocalBounds();
    m_playText.setOrigin(sf::Vector2f(playBounds.position.x + playBounds.size.x / 2.f,
                                      playBounds.position.y + playBounds.size.y / 2.f));
    m_playText.setPosition(sf::Vector2f(m_window.getSize().x / 2.f, 250));

    m_pauseText.setCharacterSize(50);
    m_pauseText.setFillColor(sf::Color::White);
    sf::FloatRect pauseBounds = m_pauseText.getLocalBounds();
    m_pauseText.setOrigin(sf::Vector2f(pauseBounds.position.x + pauseBounds.size.x / 2.f,
                                       pauseBounds.position.y + pauseBounds.size.y / 2.f));
    m_pauseText.setPosition(sf::Vector2f(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f));

    m_controlsText.setCharacterSize(20);
    m_controlsText.setFillColor(sf::Color::White);
    sf::FloatRect controlsBounds = m_controlsText.getLocalBounds();
    m_controlsText.setOrigin(sf::Vector2f(controlsBounds.position.x + controlsBounds.size.x / 2.f,
                                          controlsBounds.position.y + controlsBounds.size.y / 2.f));
    m_controlsText.setPosition(sf::Vector2f(m_window.getSize().x / 2.f, 350));

    m_scoreText.setCharacterSize(20);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setPosition(sf::Vector2f(10, 10));

    m_levelText.setCharacterSize(20);
    m_levelText.setFillColor(sf::Color::White);
    m_levelText.setPosition(sf::Vector2f(10, 35));

    m_livesText.setCharacterSize(20);
    m_livesText.setFillColor(sf::Color::White);
    m_livesText.setPosition(sf::Vector2f(10, 60));

    m_healthBarBackground.setSize(sf::Vector2f(200, 20));
    m_healthBarBackground.setFillColor(sf::Color(50, 50, 50));
    m_healthBarBackground.setPosition(sf::Vector2f(10, m_window.getSize().y - 60));

    m_healthBar.setSize(sf::Vector2f(200, 20));
    m_healthBar.setFillColor(sf::Color::Green);
    m_healthBar.setPosition(m_healthBarBackground.getPosition());

    m_shieldBarBackground.setSize(sf::Vector2f(200, 20));
    m_shieldBarBackground.setFillColor(sf::Color(50, 50, 50));
    m_shieldBarBackground.setPosition(sf::Vector2f(10, m_window.getSize().y - 30));

    m_shieldBar.setSize(sf::Vector2f(200, 20));
    m_shieldBar.setFillColor(sf::Color::Cyan);
    m_shieldBar.setPosition(m_shieldBarBackground.getPosition());
}

void ScreenSystem::update(const GameState& gameState, const int& score,
                         const ComponentMap<PlayerHealthComponent>& playerHealths,
                         const ComponentMap<ShieldComponent>& shields,
                         const ComponentMap<PlayerLivesComponent>& playerLives) {
    if (!playerLives.empty()) {
        m_livesText.setString("Lives: " + std::to_string(playerLives.begin()->second.lives));
    }
    m_scoreText.setString("Score: " + std::to_string(score));

    if (gameState == GameState::TitleScreen) {
        m_window.draw(m_titleText);
        m_window.draw(m_playText);
        m_window.draw(m_controlsText);
    } else if (gameState == GameState::Running) {
        auto playerHealthIt = playerHealths.find(1);
        if (playerHealthIt != playerHealths.end()) {
            float healthRatio = playerHealthIt->second.currentHealth / playerHealthIt->second.maxHealth;
            if (healthRatio < 0.f) healthRatio = 0.f;
            m_healthBar.setSize(sf::Vector2f(200.f * healthRatio, 20.f));
        }

        auto playerShieldIt = shields.find(1);
        if (playerShieldIt != shields.end()) {
            float shieldRatio = playerShieldIt->second.currentShield / playerShieldIt->second.maxShield;
            if (shieldRatio < 0.f) shieldRatio = 0.f;
            m_shieldBar.setSize(sf::Vector2f(200.f * shieldRatio, 20.f));
        }

        m_window.draw(m_healthBarBackground);
        m_window.draw(m_healthBar);
        m_window.draw(m_shieldBarBackground);
        m_window.draw(m_shieldBar);
        m_window.draw(m_scoreText);
        m_window.draw(m_levelText);
        m_window.draw(m_livesText);
    } else if (gameState == GameState::Paused) {
        m_window.draw(m_pauseText);
    }
}
