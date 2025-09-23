#include "CleanUpSystem.hpp"
#include <vector>
#include <iostream>

void CleanUpSystem::update(std::vector<EntityId>& entities, ComponentMap<ActiveComponent>& activeStates, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<BouncingComponent>& bouncingShapes, ComponentMap<DamageComponent>& damageValues, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerLivesComponent>& playerLives, ComponentMap<PlayerInputComponent>& playerInputs, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<SoundComponent>& sounds) {
    const float windowWidth = 800.0f;
    const float windowHeight = 600.0f;
    const float projectileRadius = 5.0f; // Matches your projectile radius in PlayerInputSystem

    // ⭐ NEW: Check for off-screen projectiles BEFORE inactive entity cleanup
    // This ensures projectiles that fly off-screen are removed even if they're still "active"
    auto projectileEnd = std::remove_if(entities.begin(), entities.end(), [&](EntityId id) {
        if (projectiles.count(id) && positions.count(id)) {
            const auto& pos = positions.at(id).position;
            // Remove projectiles that are off-screen (top, bottom, left, right)
            if (pos.y < -projectileRadius || pos.y > windowHeight + projectileRadius ||
                pos.x < -projectileRadius || pos.x > windowWidth + projectileRadius) {
                std::cout << "CleanUpSystem: Removed off-screen projectile " << id << " at (" << pos.x << ", " << pos.y << ")" << std::endl;
                // Remove all components for this projectile
                projectiles.erase(id);
                activeStates.erase(id);
                positions.erase(id);
                velocities.erase(id);
                shapes.erase(id);
                sounds.erase(id);
                damageValues.erase(id);
                return true;
            }
        }
        return false;
    });

    entities.erase(projectileEnd, entities.end());

    // ⭐ ORIGINAL: Inactive entity cleanup (for destroyed entities from CombatSystem)
    auto newEnd = std::remove_if(entities.begin(), entities.end(), [&](EntityId id) {
        if (activeStates.count(id) && !activeStates.at(id).active) {
            // Found inactive entity, remove its components
            if (projectiles.count(id)) {
                projectiles.erase(id);
            }
            if (bouncingShapes.count(id)) {
                bouncingShapes.erase(id);
            }
            if (damageValues.count(id)) {
                damageValues.erase(id);
            }
            if (playerHealths.count(id)) {
                playerHealths.erase(id);
            }
            if (shields.count(id)) {
                shields.erase(id);
            }
            if (playerLives.count(id)) {
                playerLives.erase(id);
            }
            if (playerInputs.count(id)) {
                playerInputs.erase(id);
            }
            if (positions.count(id)) {
                positions.erase(id);
            }
            if (velocities.count(id)) {
                velocities.erase(id);
            }
            if (shapes.count(id)) {
                shapes.erase(id);
            }
            if (sounds.count(id)) {
                sounds.erase(id);
            }
            activeStates.erase(id);
            std::cout << "CleanUpSystem: Removed inactive entity " << id << std::endl;
            return true; // Mark for removal from the entities vector
        }
        return false;
    });

    entities.erase(newEnd, entities.end());
}
