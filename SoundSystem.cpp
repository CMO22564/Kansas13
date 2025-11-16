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
     if (!shieldAlarmBuffer.loadFromFile("assets/alarm.wav")) {
        std::cerr << "Error loading alarm.wav" << std::endl;
    }
    if (!playerHitBuffer.loadFromFile("assets/shields.wav")) {
        std::cerr << "Error loading playerhit.wav" << std::endl;
    }
     if (!playerRespawnBuffer.loadFromFile("assets/respawn.wav")) {
        std::cerr << "Error loading respawn.wav" << std::endl;
    }
    if (!gameOverBuffer.loadFromFile("assets/gameover.wav")) {
        std::cerr << "Error loading gameover.wav" << std::endl;
    }
}

void SoundSystem::update(ComponentMap<SoundComponent>& sounds, float dt) { // <--- FIX: Add float dt
    for (auto& pair : sounds) {
        if (pair.second.type == SoundComponent::Type::Laser) {
            activeSounds.emplace_back(laserBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::Explosion) {
            activeSounds.emplace_back(explosionBuffer);
            activeSounds.back().setVolume(90.f); 
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::ShieldHit) {
            activeSounds.emplace_back(shieldBuffer);
            activeSounds.back().setVolume(130.f);
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::ShieldWarning) {
            // ShieldWarning sound is placed into activeSounds for duration check
            activeSounds.emplace_back(shieldAlarmBuffer);
            activeSounds.back().setVolume(100.f); 
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::PlayerHit) {
            activeSounds.emplace_back(playerHitBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::Respawn) {
            activeSounds.emplace_back(playerRespawnBuffer);
            activeSounds.back().play();
        } else if (pair.second.type == SoundComponent::Type::GameOver) {
            activeSounds.emplace_back(gameOverBuffer);
            activeSounds.back().play();
        }
    }

    sounds.clear();

    for (auto it = activeSounds.begin(); it != activeSounds.end();) {
        
        if (it->getStatus() == sf::SoundSource::Status::Playing) {
            
            // 🛑 CRITICAL FIX: Use & to get the address of the buffer returned by getBuffer()
            // This ensures both sides of the == are pointers (const sf::SoundBuffer*).
            if (&it->getBuffer() == &shieldAlarmBuffer) { // <--- FIX APPLIED HERE
                
                // Stop the sound if it has been playing for 5.0 seconds or more
                if (it->getPlayingOffset().asSeconds() >= 5.0f) {
                    it->stop();
                }
            }
        }
        
        // Original cleanup logic
        if (it->getStatus() == sf::SoundSource::Status::Stopped) {
            it = activeSounds.erase(it);
        } else {
            ++it;
        }
    }
}
