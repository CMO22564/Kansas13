#pragma once
#include <vector>
#include "Core.hpp"

class CleanUpSystem {
public:
    // Fix: Update the function signature with the new component names
    void update(std::vector<EntityId>& entities, ComponentMap<ActiveComponent>& activeStates, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<BouncingComponent>& bouncingShapes, ComponentMap<DamageComponent>& damageValues, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerLivesComponent>& playerLives, ComponentMap<PlayerInputComponent>& playerInputs, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<SoundComponent>& sounds);
};
