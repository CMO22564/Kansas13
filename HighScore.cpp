#include "HighScore.hpp"
#include <iostream>

void HighScoreManager::loadScores() {
    m_scores.clear();
    std::ifstream file(m_filename);
    if (!file.is_open()) {
        // Initialize with default scores if file doesn't exist
        for (size_t i = 0; i < MAX_SCORES; ++i) {
            m_scores.push_back(ScoreEntry{"---", 0});
        }
        return;
    }

    try {
        nlohmann::json j;
        file >> j;
        m_scores = j.get<std::vector<ScoreEntry>>();
        // Ensure we have exactly MAX_SCORES entries
        while (m_scores.size() < MAX_SCORES) {
            m_scores.push_back(ScoreEntry{"---", 0});
        }
        // Sort scores in descending order
        std::sort(m_scores.begin(), m_scores.end(), std::greater<ScoreEntry>());
        m_scores.resize(MAX_SCORES);
    } catch (const std::exception& e) {
        std::cerr << "Error loading high scores: " << e.what() << std::endl;
        // Fallback to default scores
        m_scores.clear();
        for (size_t i = 0; i < MAX_SCORES; ++i) {
            m_scores.push_back(ScoreEntry{"---", 0});
        }
    }
    file.close();
}

void HighScoreManager::saveScores() {
    try {
        std::ofstream file(m_filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open highscores.json for writing" << std::endl;
            return;
        }
        nlohmann::json j = m_scores;
        file << j.dump(4); // Pretty print with 4 spaces
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error saving high scores: " << e.what() << std::endl;
    }
}

void HighScoreManager::addScore(const std::string& initials, int score) {
    m_scores.push_back(ScoreEntry{initials, score});
    std::sort(m_scores.begin(), m_scores.end(), std::greater<ScoreEntry>());
    if (m_scores.size() > MAX_SCORES) {
        m_scores.resize(MAX_SCORES);
    }
    saveScores();
}