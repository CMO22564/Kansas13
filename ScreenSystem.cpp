#include "ScreenSystem.hpp"
#include "GameStateManager.hpp"
#include "HighScore.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

#include "ScreenSystem.hpp"
#include "GameStateManager.hpp"
#include "HighScore.hpp"
#include <sstream>
#include <iomanip>
#include <iostream> // Include for error messages

// CORRECTED: Constructor signature must match the header (ScreenSystem(sf::RenderWindow&, const std::string&))
// All non-default-constructible members (sf::Text) MUST be initialized here.
ScreenSystem::ScreenSystem(sf::RenderWindow& window, const std::string& fontPath)
    // 1. Initialize reference member (must be first)
    : m_window(window),
    // 2. Initialize all sf::Text members using m_font (even though it's empty for now)
      m_titleText(m_font, "", 30),
      m_playText(m_font, "", 30),
      m_pauseText(m_font, "", 30),
      m_controlsText(m_font, "", 30),
      m_scoreText(m_font, "", 30),
      m_levelText(m_font, "", 30),
      m_livesText(m_font, "", 30),
      m_gameOverText(m_font, "", 30)
      // NOTE: Other members (m_font, m_healthBarBackground, etc.) are default-constructed implicitly
      // unless you have a non-default constructor for them, in which case add them here.
{
    // Now the constructor body runs, and all members exist.
    
    // 3. Load the font file
    if (!m_font.openFromFile(fontPath)) {
        std::cerr << "Error: Could not load font from path: " << fontPath << std::endl;
        // Fatal error, but proceed to avoid crashing later, assuming the code handles an empty font.
    }

    // 4. Set the font for ALL sf::Text members again (since they were initialized with an empty font)
    //    and set their initial properties.

    // Title Text setup (Lines 22-27 in your original body)
    m_titleText.setFont(m_font);
    m_titleText.setString("KANSAS 13");
    m_titleText.setCharacterSize(60);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
    
    // Play Text setup (Lines 28-33)
    m_playText.setFont(m_font);
    m_playText.setString("Press P to Play");
    m_playText.setCharacterSize(40);
    m_playText.setFillColor(sf::Color::Green);

    // Pause Text setup (Lines 34-39)
    m_pauseText.setFont(m_font);
    m_pauseText.setString("PAUSED");
    m_pauseText.setCharacterSize(60);
    m_pauseText.setFillColor(sf::Color::Red);

    // Controls Text setup (Lines 40-45)
    m_controlsText.setFont(m_font);
    m_controlsText.setString("Controls: Arrows/WASD to Move, Space to Shoot, ESC to Pause");
    m_controlsText.setCharacterSize(20);
    m_controlsText.setFillColor(sf::Color::White);
    
    // Score Text setup (Lines 46-51)
    m_scoreText.setFont(m_font);
    m_scoreText.setString("Score: 0"); // Initial score
    m_scoreText.setCharacterSize(24);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setPosition(sf::Vector2f(10.f, 10.f));

    // Lives Text setup (Lines 52-56)
    m_livesText.setFont(m_font);
    m_livesText.setString("Lives: 3"); // Initial lives
    m_livesText.setCharacterSize(24);
    m_livesText.setFillColor(sf::Color::Green);
    m_livesText.setPosition(sf::Vector2f(10.f, 40.f));
    
    // Game Over Text setup (Lines 57-62)
    m_gameOverText.setFont(m_font);
    m_gameOverText.setString("GAME OVER");
    m_gameOverText.setCharacterSize(60);
    m_gameOverText.setFillColor(sf::Color::Red);
    m_gameOverText.setStyle(sf::Text::Bold);

    // Initialize Rectangles (if needed, otherwise default is fine)
    m_healthBarBackground.setSize({150.f, 10.f});
    m_healthBarBackground.setFillColor(sf::Color::Red);
    m_healthBarBackground.setPosition(sf::Vector2f(10.f, 70.f)); 
    
    m_healthBar.setSize({150.f, 10.f});
    m_healthBar.setFillColor(sf::Color::Green);
    m_healthBar.setPosition(sf::Vector2f(10.f, 70.f));

    m_shieldBarBackground.setSize({150.f, 10.f});
    m_shieldBarBackground.setFillColor(sf::Color(100, 100, 100)); // Dark Gray
    m_shieldBarBackground.setPosition(sf::Vector2f(10.f, 85.f));

    m_shieldBar.setSize({150.f, 10.f});
    m_shieldBar.setFillColor(sf::Color::Blue);
    m_shieldBar.setPosition(sf::Vector2f(10.f, 85.f));

}
// ... rest of ScreenSystem.cpp functions (update, render, etc.) will now find the members.

// --- Update Method (Handles HUD Data Logic) ---
void ScreenSystem::update(const GameState& gameState, const int& score,
                          const ComponentMap<PlayerHealthComponent>& playerHealths,
                          const ComponentMap<ShieldComponent>& shields,
                          const ComponentMap<PlayerLivesComponent>& playerLives) {
    // Only update HUD elements when the game is running or paused
    if (gameState != GameState::Running && gameState != GameState::Paused) return;

    // Update Score Text
    std::ostringstream scoreStream;
    scoreStream << "Score: " << score;
    m_scoreText.setString(scoreStream.str());

    // Assuming a single player entity exists (playerId = 1)
    EntityId playerId = 1; 

    // Update Lives Text
    if (playerLives.count(playerId)) {
        std::ostringstream livesStream;
        livesStream << "Lives: " << playerLives.at(playerId).lives;
        m_livesText.setString(livesStream.str());
    }

    // Update Health Bar
    if (playerHealths.count(playerId)) {
        const auto& health = playerHealths.at(playerId);
        float healthRatio = health.currentHealth / health.maxHealth;
        float barWidth = healthRatio * m_healthBarBackground.getSize().x;
        m_healthBar.setSize(sf::Vector2f(barWidth, m_healthBar.getSize().y));
    }

    // Update Shield Bar
    if (shields.count(playerId)) {
        const auto& shield = shields.at(playerId);
        float shieldRatio = shield.currentShield / shield.maxShield;
        
        // Change color to Yellow/Red if under 25% (visual warning)
        if (shieldRatio <= 0.25f && shieldRatio > 0.0f) {
            m_shieldBar.setFillColor(sf::Color::Yellow);
        } else if (shieldRatio <= 0.0f) {
            m_shieldBar.setFillColor(sf::Color::Red);
        } else {
            m_shieldBar.setFillColor(sf::Color::Cyan); // Back to normal color
        }
        
        float barWidth = shieldRatio * m_shieldBarBackground.getSize().x;
        m_shieldBar.setSize(sf::Vector2f(barWidth, m_shieldBar.getSize().y));
    }
}

// --- Render Method (Handles Screen State Drawing) ---
void ScreenSystem::render(sf::RenderWindow& window, GameStateManager& manager) {
    // FIX 2: Use getState() instead of getCurrentState()
    GameState state = manager.getState(); 
    
    // Always draw HUD elements during Running and Paused states
    if (state == GameState::Running || state == GameState::Paused) {
        window.draw(m_scoreText);
        window.draw(m_healthBarBackground);
        window.draw(m_healthBar);
        window.draw(m_shieldBarBackground);
        window.draw(m_shieldBar);
        window.draw(m_livesText);
    }
    
    switch (state) {
        case GameState::TitleScreen:
            renderTitleScreen(window);
            break;
            
        // FIX 3: Use GameState::Running instead of GameState::Playing
        case GameState::Running:
            // HUD is drawn above, no overlay is needed
            break;
            
        case GameState::Paused:
            renderPauseScreen(window);
            break;
            
        case GameState::GameOver:
            renderGameOverScreen(window, manager);
            break;
            
        case GameState::ScoreEntry:
            renderScoreEntryScreen(window, manager);
            break;
            
        case GameState::LevelComplete:
            // Assuming LevelTransition is defined in GameState.hpp
            renderLevelTransitionScreen(window, manager);
            break;
    }
}

// --- Helper Render Functions (Definitions) ---

void ScreenSystem::renderTitleScreen(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    
    // Semi-transparent overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Draw Title
    window.draw(m_titleText);
    
    // Draw Play Prompt
    m_playText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f)); // FIX: sf::Vector2f
    sf::FloatRect playBounds = m_playText.getLocalBounds();
    m_playText.setOrigin(sf::Vector2f(playBounds.size.x / 2.f, playBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    window.draw(m_playText);

    // Draw Controls
    m_controlsText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y - 50.f)); // FIX: sf::Vector2f
    sf::FloatRect controlsBounds = m_controlsText.getLocalBounds();
    m_controlsText.setOrigin(sf::Vector2f(controlsBounds.size.x / 2.f, controlsBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    window.draw(m_controlsText);
}

void ScreenSystem::renderPauseScreen(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    
    // Semi-transparent overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Draw Pause text
    sf::FloatRect pauseBounds = m_pauseText.getLocalBounds();
    m_pauseText.setOrigin(sf::Vector2f(pauseBounds.size.x / 2.f, pauseBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    m_pauseText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f)); // FIX: sf::Vector2f
    window.draw(m_pauseText);
}

void ScreenSystem::renderGameOverScreen(sf::RenderWindow& window, GameStateManager& manager) {
    sf::Vector2u windowSize = window.getSize();
    
    // Semi-transparent overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    // Draw Game Over text
    window.draw(m_gameOverText);

    // Display Final Score
    std::ostringstream scoreStream;
    scoreStream << "Final Score: " << manager.getScore();
    // FIX: Correct sf::Text constructor order: (Font, String, Size)
    sf::Text finalScoreText(m_font, scoreStream.str(), 36); 
    finalScoreText.setFillColor(sf::Color::White);
    
    sf::FloatRect bounds = finalScoreText.getLocalBounds();
    finalScoreText.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    finalScoreText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f + 50.f)); // FIX: sf::Vector2f
    window.draw(finalScoreText);
    
    // Prompt to return to title screen
    // FIX: Correct sf::Text constructor order: (Font, String, Size)
    sf::Text promptText(m_font, "Press P for Title Screen", 24); 
    promptText.setFillColor(sf::Color::Yellow);
    
    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(sf::Vector2f(promptBounds.size.x / 2.f, promptBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    promptText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f + 120.f)); // FIX: sf::Vector2f
    window.draw(promptText);
}

void ScreenSystem::renderScoreEntryScreen(sf::RenderWindow& window, GameStateManager& manager) {
    sf::Vector2u windowSize = window.getSize();
    
    // Semi-transparent overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    // Prompt for initials
    sf::Text promptText(m_font, "Enter Initials:", 40);
    promptText.setFillColor(sf::Color::Cyan);
    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(sf::Vector2f(promptBounds.size.x / 2.f, promptBounds.size.y / 2.f)); // FIX: sf::Vector2f
    promptText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 3.f)); // FIX: sf::Vector2f
    window.draw(promptText);
    
    // Display initials (using getCurrentInitials from GameStateManager)
    // FIX: Correct sf::Text constructor order: (Font, String, Size)
    sf::Text initialsText(m_font, manager.getCurrentInitials(), 60); 
    initialsText.setFillColor(sf::Color::Yellow);
    sf::FloatRect initialsBounds = initialsText.getLocalBounds();
    initialsText.setOrigin(sf::Vector2f(initialsBounds.size.x / 2.f, initialsBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    initialsText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f)); // FIX: sf::Vector2f
    window.draw(initialsText);
    
    // Display High Scores (Top 10)
    // FIX: Correct sf::Text constructor order: (Font, String, Size)
    sf::Text scoreListTitle(m_font, "HIGH SCORES", 28);
    scoreListTitle.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = scoreListTitle.getLocalBounds();
    scoreListTitle.setOrigin(sf::Vector2f(titleBounds.size.x / 2.f, 0.f)); // FIX: sf::Vector2f
    scoreListTitle.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f + 80.f)); // FIX: sf::Vector2f
    window.draw(scoreListTitle);
    
    // Draw the list of scores
    float yPos = scoreListTitle.getPosition().y + titleBounds.size.y + 10.f; // FIX: use .size.y for height
    for (const auto& entry : manager.getHighScoreManager().getTopScores()) {
        std::ostringstream entryStream;
        entryStream << entry.initials << " - " << entry.score;
        // FIX: Correct sf::Text constructor order: (Font, String, Size)
        sf::Text entryText(m_font, entryStream.str(), 24); 
        
        sf::FloatRect entryBounds = entryText.getLocalBounds();
        entryText.setOrigin(sf::Vector2f(entryBounds.size.x / 2.f, entryBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
        entryText.setPosition(sf::Vector2f(windowSize.x / 2.f, yPos)); // FIX: sf::Vector2f
        
        window.draw(entryText);
        yPos += 30.f;
    }
}

void ScreenSystem::renderLevelTransitionScreen(sf::RenderWindow& window, GameStateManager& manager) {
    sf::Vector2u windowSize = window.getSize();
    
    // Semi-transparent overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Level Complete text
    // FIX: Correct sf::Text constructor order: (Font, String, Size)
    sf::Text levelCompleteText(m_font, "LEVEL COMPLETE!", 60); 
    levelCompleteText.setFillColor(sf::Color::Green);
    levelCompleteText.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = levelCompleteText.getLocalBounds();
    levelCompleteText.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f)); // FIX: sf::Vector2f
    levelCompleteText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 3.f)); // FIX: sf::Vector2f
    window.draw(levelCompleteText);
    
    // Next level info
    std::ostringstream nextLevelStream;
    nextLevelStream << "Advancing to Level " << (manager.getCurrentLevelIndex() + 1); 
    // FIX: Correct sf::Text constructor order: (Font, String, Size)
    sf::Text nextLevelText(m_font, nextLevelStream.str(), 36);
    nextLevelText.setFillColor(sf::Color::White);
    sf::FloatRect nextBounds = nextLevelText.getLocalBounds();
    nextLevelText.setOrigin(sf::Vector2f(nextBounds.size.x / 2.f, nextBounds.size.y / 2.f)); // FIX: use .size.x/.size.y and sf::Vector2f
    nextLevelText.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f)); // FIX: sf::Vector2f
    window.draw(nextLevelText);
}