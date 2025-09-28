#pragma once

#include "Core.hpp" // This is the fix.
#include <vector>
#include "GameStateManager.hpp"

class CombatSystem {
public:
    void update(std::vector<EntityId>& entities,
                ComponentMap<PositionComponent>& positions,
                ComponentMap<RenderComponent>& shapes,
                ComponentMap<ProjectileComponent>& projectiles,
                ComponentMap<BouncingComponent>& bouncingShapes,
                ComponentMap<DamageComponent>& damageValues,
                ComponentMap<ActiveComponent>& activeStates,
                ComponentMap<PlayerHealthComponent>& playerHealths,
                ComponentMap<HealthComponent>& healths, // This is the line you need to add back
                ComponentMap<ShieldComponent>& shields,
                ComponentMap<SoundComponent>& sounds,
                ComponentMap<VelocityComponent>& velocities);
};
