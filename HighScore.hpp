#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <json/json.hpp> // Assuming you use nlohmann/json

struct ScoreEntry {
    std::string initials = "---";
    int score = 0;

    // Helper for sorting
    bool operator>(const ScoreEntry& other) const {
        return score > other.score;
    }

    // For JSON serialization
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScoreEntry, initials, score)
};

class HighScoreManager {
public:
    HighScoreManager() { loadScores(); }
    void loadScores();
    void saveScores();
    bool isHighScore(int score) const { return m_scores.empty() || score > m_scores.back().score; }
    void addScore(const std::string& initials, int score);
    const std::vector<ScoreEntry>& getTopScores() const { return m_scores; }

private:
    std::vector<ScoreEntry> m_scores;
    const std::string m_filename = "highscores.json";
    const size_t MAX_SCORES = 10;
};