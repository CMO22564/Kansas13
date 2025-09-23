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
    sf::SoundBuffer playerHitBuffer;
    std::list<sf::Sound> activeSounds;
public:
    SoundSystem();
    void update(ComponentMap<SoundComponent>& sounds);
};
