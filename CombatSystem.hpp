#pragma once
#include "Core.hpp"
#include <vector>
#include "GameStateManager.hpp" // Include for singleton access

class CombatSystem {
public:
    void update(std::vector<EntityId>& entities,
                ComponentMap<PositionComponent>& positions,
                ComponentMap<ShapeComponent>& shapes,
                ComponentMap<ProjectileComponent>& projectiles,
                ComponentMap<BouncingComponent>& bouncingShapes,
                ComponentMap<DamageComponent>& damageValues,
                ComponentMap<ActiveComponent>& activeStates,
                ComponentMap<PlayerHealthComponent>& playerHealths,
                ComponentMap<ShieldComponent>& shields,
                ComponentMap<SoundComponent>& sounds,
                ComponentMap<VelocityComponent>& velocities);
};
