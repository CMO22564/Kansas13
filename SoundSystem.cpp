#include "SoundSystem.hpp"
#include <iostream>

SoundSystem::SoundSystem() {
    if (!laserBuffer.loadFromFile("assets/laser.wav")) {
        std::cerr << "Error loading laser.wav" << std::endl;
    }
    if (!explosionBuffer.loadFromFile("assets/explosion.wav")) {
        std::cerr << "Error loading explosion.wav" << std::endl;
    }
    if (!shieldBuffer.loadFromFile("assets/shields.wav")) {
        std::cerr << "Error loading shields.wav" << std::endl;
    }
    if (!playerHitBuffer.loadFromFile("assets/shields.wav")) {
        std::cerr << "Error loading playerhit.wav" << std::endl;
    }
}

void SoundSystem::update(ComponentMap<SoundComponent>& sounds) {
    for (auto& pair : sounds) {
        if (pair.second.type == SoundComponent::Type::Laser) {
            activeSounds.emplace_back(laserBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::Explosion) {
            activeSounds.emplace_back(explosionBuffer);
            activeSounds.back().setVolume(90.f); // Set volume to 90% 
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::ShieldHit) {
            activeSounds.emplace_back(shieldBuffer);
            activeSounds.back().setVolume(130.f); // Set volume to 130% (for emphasis)
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::PlayerHit) {
            activeSounds.emplace_back(playerHitBuffer);
            activeSounds.back().play();
        }
    }
    sounds.clear();

    for (auto it = activeSounds.begin(); it != activeSounds.end();) {
        if (it->getStatus() == sf::SoundSource::Status::Stopped) {
            it = activeSounds.erase(it);
        } else {
            ++it;
        }
    }
}

