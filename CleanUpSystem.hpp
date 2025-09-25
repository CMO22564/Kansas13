#pragma once
#include <vector>
#include "Core.hpp"

class CleanUpSystem {
public:
    // Fix: Update the function signature with the new component names
    void update(std::vector<EntityId>& entities, ComponentMap<ActiveComponent>& activeStates, ComponentMap<ProjectileComponent>& projectiles, ComponentMap<BouncingComponent>& bouncingShapes, ComponentMap<DamageComponent>& damageValues, ComponentMap<PlayerHealthComponent>& playerHealths, ComponentMap<ShieldComponent>& shields, ComponentMap<PlayerLivesComponent>& playerLives, ComponentMap<PlayerInputComponent>& playerInputs, ComponentMap<PositionComponent>& positions, ComponentMap<VelocityComponent>& velocities, ComponentMap<ShapeComponent>& shapes, ComponentMap<SoundComponent>& sounds);


 // NEW: Function to clear the game world for a level transition
    void clearNonPlayerEntities(std::vector<EntityId>& entities, 
                                ComponentMap<ProjectileComponent>& projectiles, 
                                ComponentMap<BouncingComponent>& bouncingShapes, 
                                ComponentMap<DamageComponent>& damageValues, 
                                ComponentMap<ActiveComponent>& activeStates,
                                ComponentMap<PositionComponent>& positions, 
                                ComponentMap<VelocityComponent>& velocities, 
                                ComponentMap<ShapeComponent>& shapes, 
                                ComponentMap<SoundComponent>& sounds);
};
