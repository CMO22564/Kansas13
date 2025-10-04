#pragma once
#include <vector>
#include "Core.hpp"
#include <unordered_map>

class CombatSystem {
public:
    void update(
        std::vector<EntityId>& entities,
        ComponentMap<PositionComponent>& positions,
        ComponentMap<RenderComponent>& shapes,
        ComponentMap<ProjectileComponent>& projectiles,
        ComponentMap<BouncingComponent>& bouncingShapes,
        ComponentMap<DamageComponent>& damages,
        ComponentMap<ActiveComponent>& actives,
        ComponentMap<PlayerHealthComponent>& playerHealths,
        ComponentMap<HealthComponent>& healths,
        ComponentMap<ShieldComponent>& shields,
        ComponentMap<SoundComponent>& sounds,
        ComponentMap<VelocityComponent>& velocities,
        ComponentMap<EnemyComponent>& enemies // <-- CRITICAL FIX: Add the new EnemyComponent map
    );
};
