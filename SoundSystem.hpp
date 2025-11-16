#pragma once
#include "Core.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <list>

class SoundSystem {
private:
    sf::SoundBuffer laserBuffer;
    sf::SoundBuffer explosionBuffer;
    sf::SoundBuffer shieldBuffer;
    sf::SoundBuffer shieldAlarmBuffer;
    sf::SoundBuffer playerHitBuffer;
    sf::SoundBuffer playerRespawnBuffer;
    sf::SoundBuffer gameOverBuffer;
    std::list<sf::Sound> activeSounds;
public:
    // 🛑 CRITICAL FIX: Add this line to declare the constructor
    SoundSystem();
void update(ComponentMap<SoundComponent>& sounds, float dt);
};
