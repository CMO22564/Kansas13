#include "ScreenSystem.hpp"
#include "GameStateManager.hpp"
#include "HighScore.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>


ScreenSystem::ScreenSystem(sf::RenderWindow& window, const std::string& fontPath)
    // 1. Initialize reference member (must be first)
    : m_window(window),

    // 2. Initialize sf::Font
      m_font(),

    // 3. Initialize all sf::Text members *after* m_font is ready.
    // We initialize them with a dummy string/size to satisfy the sf::Text constructor.
      m_titleText(m_font, "Title", 30),
      m_playText(m_font, "Play", 30),
      m_pauseText(m_font, "Paused", 30),
      m_controlsText(m_font, "Controls", 30),
      m_scoreText(m_font, "Score", 30),
      m_levelText(m_font, "Level", 30), 
      m_livesText(m_font, "Lives", 30),
      m_creditsText(m_font, "Credits", 30), // Initialize here
      m_gameOverText(m_font, "Game Over", 30)
    // NOTE: Other shape/rectangle members are default-constructed implicitly.
{
    // Now the constructor body runs, and all members exist.
    
    // 4. Load the font file
    if (!m_font.openFromFile(fontPath)) {
        std::cerr << "Error: Could not load font from path: " << fontPath << std::endl;
        // Fatal error, but proceed to avoid crashing later, assuming the code handles an empty font.
    }

    // 5. Set the font for ALL sf::Text members again (since they were initialized with an empty font)
    //    and set their initial properties.

    // Title Text setup (Lines 22-27 in your original body)
    m_titleText.setFont(m_font);
    m_titleText.setString("CHAOS 13");
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
    m_controlsText.setString("Controls: Arrows <- -> / A D to Move, Space to Shoot, ESC to Pause");
    m_controlsText.setCharacterSize(20);
    m_controlsText.setFillColor(sf::Color::White);
    
    // Score Text setup (Lines 46-51)
    m_scoreText.setFont(m_font);
    m_scoreText.setString("Score: 0"); // Initial score
    m_scoreText.setCharacterSize(24);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setPosition(sf::Vector2f(10.f, 10.f));

    // Level Text setup (Lines 52-56)
    m_levelText.setFont(m_font);
    m_levelText.setString("Level: 1"); // Initial level
    m_levelText.setCharacterSize(24);
    m_levelText.setFillColor(sf::Color::Green);
    m_levelText.setPosition(sf::Vector2f(10.f, 40.f));

    // Lives Text setup (Lines 57-62)
    m_livesText.setFont(m_font);
    m_livesText.setString("Lives: 3"); // Initial lives
    m_livesText.setCharacterSize(24);
    m_livesText.setFillColor(sf::Color::Green);
    m_livesText.setPosition(sf::Vector2f(10.f, 70.f));
    
    // Game Over Text setup (Lines 57-62)
    m_gameOverText.setFont(m_font);
    m_gameOverText.setString("GAME OVER");
    m_gameOverText.setCharacterSize(60);
    m_gameOverText.setFillColor(sf::Color::Red);
    m_gameOverText.setStyle(sf::Text::Bold);

    // Initialize Rectangles (if needed, otherwise default is fine)
    m_healthBarBackground.setSize({150.f, 10.f});
    m_healthBarBackground.setFillColor(sf::Color::Red);
    m_healthBarBackground.setPosition(sf::Vector2f(10.f, 100.f)); 
    
    m_healthBar.setSize({150.f, 10.f});
    m_healthBar.setFillColor(sf::Color::Green);
    m_healthBar.setPosition(sf::Vector2f(10.f, 100.f));

    m_shieldBarBackground.setSize({150.f, 10.f});
    m_shieldBarBackground.setFillColor(sf::Color(100, 100, 130)); // Dark Gray
    m_shieldBarBackground.setPosition(sf::Vector2f(10.f, 115.f));

    m_shieldBar.setSize({150.f, 10.f});
    m_shieldBar.setFillColor(sf::Color::Blue);
    m_shieldBar.setPosition(sf::Vector2f(10.f, 115.f));

    // Credits Text Setup
    m_creditsText.setFont(m_font);
    m_creditsText.setString("Credits: 0");
    m_creditsText.setCharacterSize(24);
    m_creditsText.setFillColor(sf::Color(255, 215, 0)); // Gold Color
    m_creditsText.setPosition(sf::Vector2f(10.f, 135.f)); // Positioned below bars

}
// ... rest of ScreenSystem.cpp functions (update, render, etc.) will now find the members.

// --- Update Method (Handles HUD Data Logic) ---
// ScreenSystem.cpp

void ScreenSystem::update(const GameState& gameState, const int& score, const int& levelIndex,
                         const ComponentMap<PlayerHealthComponent>& playerHealths,
                         const ComponentMap<ShieldComponent>& shields,
                         const ComponentMap<PlayerLivesComponent>& playerLives,
                         const ComponentMap<PlayerInputComponent>& playerInputs) { // Added playerInputs
    
    // 1. Update general HUD text
    m_scoreText.setString("Score: " + std::to_string(score));
    m_levelText.setString("Level: " + std::to_string(levelIndex + 1));

    // 2. Iterate through player entities to update specific HUD elements
    // This loop defines 'id' for use in all subsequent checks
    for (auto const& [id, health] : playerHealths) {
        
        // Update Health Bar logic
        float healthRatio = (health.maxHealth > 0) ? health.currentHealth / health.maxHealth : 0.0f;
        m_healthBar.setSize({150.f * healthRatio, 10.f});

        // Update Shield Bar (if the player has a shield component)
        if (shields.count(id)) {
            const auto& shield = shields.at(id);
            float shieldRatio = (shield.maxShield > 0) ? shield.currentShield / shield.maxShield : 0.0f;
            m_shieldBar.setSize({150.f * shieldRatio, 10.f});
        }

        // Update Player Lives text
        if (playerLives.count(id)) {
            m_livesText.setString("Lives: " + std::to_string(playerLives.at(id).lives));
        }

        // 🌟 FIX: Update Credits UI (id is now correctly in scope)
        if (playerInputs.count(id)) {
            m_creditsText.setString("Credits: " + std::to_string(playerInputs.at(id).credits));
        }
        
        // Assuming single-player, we break after the first valid player entity
        break; 
    }
}

// --- Render Method (Handles Screen State Drawing) ---
void ScreenSystem::render(sf::RenderWindow& window, GameStateManager& manager) {
    
    GameState state = manager.getState(); 
    
    // Always draw HUD elements during Running and Paused states
    if (state == GameState::Running || state == GameState::Paused) {
        window.draw(m_scoreText);
        // 🌟 ADDED: Draw the Level Text
        window.draw(m_levelText); 
        window.draw(m_livesText);
        window.draw(m_healthBarBackground);
        window.draw(m_healthBar);
        window.draw(m_shieldBarBackground);
        window.draw(m_shieldBar);
    }
    
    switch (state) {
        case GameState::TitleScreen:
            renderTitleScreen(window, manager);
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

void ScreenSystem::renderTitleScreen(sf::RenderWindow& window, GameStateManager& manager) {
    sf::Vector2u windowSize = window.getSize();
    
    // --- VAPOR WAVE COLORS ---
    const sf::Color VAPOR_PINK(255, 0, 255);      // Hot Pink / Magenta
    const sf::Color VAPOR_BLUE(0, 255, 255);      // Cyan / Electric Blue
    const sf::Color VAPOR_ORANGE(255, 128, 0);    // Retro Orange
    
    // Semi-transparent overlay 
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 200)); 
    window.draw(overlay);
    
    // ----------------------------------------------------------------
    // Title: "KANSAS 13" (Neon Glow Effect)
    // ----------------------------------------------------------------
    const std::string titleString = "CHAOS 13";
    float titleY = windowSize.y / 3.f;

    // 1. Draw the Shadow/Glow layer (Vapor Blue, slightly offset)
    // FIX 1: Corrected constructor order: (font, string, size)
    sf::Text glowText(m_font, titleString, 96);
    glowText.setFillColor(VAPOR_BLUE);
    glowText.setStyle(sf::Text::Bold);
    sf::FloatRect glowBounds = glowText.getLocalBounds();
    // FIX 2: Corrected setOrigin/setPosition to use sf::Vector2f initializer list {x, y}
    glowText.setOrigin({glowBounds.size.x / 2.f, glowBounds.size.y / 2.f});
    glowText.setPosition({windowSize.x / 2.f + 4.f, titleY + 4.f}); // Offset by 4 pixels
    window.draw(glowText);

    // 2. Draw the primary text (Vapor Pink)
    // FIX 1: Corrected constructor order: (font, string, size)
    sf::Text title(m_font, titleString, 96);
    title.setFillColor(VAPOR_PINK); 
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    // FIX 2: Corrected setOrigin/setPosition to use sf::Vector2f initializer list {x, y}
    title.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    title.setPosition({windowSize.x / 2.f, titleY});
    window.draw(title);
    
    // ----------------------------------------------------------------
    // Instructions: "Press 'P' to Start" 
    // ----------------------------------------------------------------
    // FIX 1: Corrected constructor order: (font, string, size)
    sf::Text instructions(m_font, "Press 'P' to Start", 36);
    instructions.setFillColor(VAPOR_ORANGE); 
    instructions.setStyle(sf::Text::Bold); 
    sf::FloatRect instBounds = instructions.getLocalBounds();
    // FIX 2: Corrected setOrigin/setPosition to use sf::Vector2f initializer list {x, y}
    instructions.setOrigin({instBounds.size.x / 2.f, instBounds.size.y / 2.f});
    instructions.setPosition({windowSize.x / 2.f, windowSize.y / 2.f});
    window.draw(instructions);
    
    // ----------------------------------------------------------------
    // Developer Info: "Developer: ONeil"
    // ----------------------------------------------------------------
    // FIX 1: Corrected constructor order: (font, string, size)
    sf::Text devInfo(m_font, "Developer: ONeil", 24);
    devInfo.setFillColor(VAPOR_BLUE); 
    sf::FloatRect devBounds = devInfo.getLocalBounds();
    // FIX 2: Corrected setOrigin/setPosition to use sf::Vector2f initializer list {x, y}
    devInfo.setOrigin({devBounds.size.x / 2.f, devBounds.size.y / 2.f});
    devInfo.setPosition({windowSize.x / 2.f, windowSize.y * 0.9f});
    window.draw(devInfo);


    // --- High Score Display ---
    const HighScoreManager& hsm = manager.getHighScoreManager();
    const auto& scores = hsm.getTopScores(); 

    sf::Text highScoreTitle(m_font, "TOP TEN", 36);
    highScoreTitle.setFillColor(VAPOR_ORANGE);
    highScoreTitle.setStyle(sf::Text::Bold);

    sf::FloatRect titleBounds = highScoreTitle.getLocalBounds();
    highScoreTitle.setOrigin(sf::Vector2f(titleBounds.size.x / 2.f, titleBounds.size.y / 2.f));

    // 1. Position the title below the instructions (assuming instructions are around 0.60f)
    highScoreTitle.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y * 0.65f)); 
    window.draw(highScoreTitle);

    // 2. Define layout variables
    float startY = windowSize.y * 0.70f;     // Starting Y position for the first score entry
    float lineHeight = 22.f;                 // Line spacing (slightly increased from 20.f for clarity)
    int scoreFontSize = 18;                  // Slightly larger font (was 16)

    // Define X positions for the two columns
    // Left Column (1/4 of the way across)
    float leftColumnX = windowSize.x * 0.35f; 
    // Right Column (3/4 of the way across)
    float rightColumnX = windowSize.x * 0.65f; 

    // --- Column 1: Scores 1 through 5 ---
    for (size_t i = 0; i < scores.size() && i < 5; ++i) {
        const auto& scoreEntry = scores[i];
        
        std::stringstream ss;
        ss << std::setw(2) << std::left << (i + 1) << ". " 
        << std::setw(3) << std::left << scoreEntry.initials << " - " 
        << std::setw(6) << std::right << scoreEntry.score;
        
        sf::Text scoreText(m_font, ss.str(), scoreFontSize); 
        scoreText.setFillColor(VAPOR_BLUE);
        
        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        // Center the text origin
        scoreText.setOrigin(sf::Vector2f(scoreBounds.size.x / 2.f, scoreBounds.size.y / 2.f));
        
        // Position using the LEFT column X and current Y
        scoreText.setPosition(sf::Vector2f(leftColumnX, startY + (float)i * lineHeight)); 
        
        window.draw(scoreText);
    }

    // --- Column 2: Scores 6 through 10 ---
    for (size_t i = 5; i < scores.size() && i < 10; ++i) {
        const auto& scoreEntry = scores[i];
        
        std::stringstream ss;
        ss << std::setw(2) << std::left << (i + 1) << ". " 
        << std::setw(3) << std::left << scoreEntry.initials << " - " 
        << std::setw(6) << std::right << scoreEntry.score;
        
        sf::Text scoreText(m_font, ss.str(), scoreFontSize); 
        scoreText.setFillColor(VAPOR_BLUE);
        
        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        // Center the text origin
        scoreText.setOrigin(sf::Vector2f(scoreBounds.size.x / 2.f, scoreBounds.size.y / 2.f));
        
        // Position using the RIGHT column X. We subtract 5 from i to reset the Y-offset.
        scoreText.setPosition(sf::Vector2f(rightColumnX, startY + (float)(i - 5) * lineHeight)); 
        
        window.draw(scoreText);
    }
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