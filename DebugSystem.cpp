#include "DebugSystem.hpp"
#include "GameStateManager.hpp"
#include <imgui.h>
#include <string>

void DebugSystem::update(int score,
                         const ComponentMap<PlayerHealthComponent>& playerHealths,
                         const ComponentMap<ShieldComponent>& shields,
                         const ComponentMap<PlayerLivesComponent>& playerLives,
                         const std::vector<EntityId>& entities,
                         const ComponentMap<PositionComponent>& positions,
                         const ComponentMap<ShapeComponent>& shapes,
                         const ComponentMap<VelocityComponent>& velocities) {
    ImGui::Begin("Debug Window");

    // Display game state information
    ImGui::Text("Score: %d", score);
    ImGui::Text("Player Lives: %d", playerLives.at(1).lives);
    ImGui::Text("Player Health: %.1f", playerHealths.at(1).currentHealth);
    ImGui::Text("Player Shield: %.1f", shields.at(1).currentShield);

    // ⭐ New: Checkbox to toggle shape visibility
    ImGui::Checkbox("Show Shapes", &showShapes);

    // ⭐ New: Button to toggle game state
   // bool isPaused = (GameStateManager::getInstance().getState() == GameState::Paused);
   // if (ImGui::Button(isPaused ? "Un-pause" : "Pause")) {
    //    GameStateManager::getInstance().togglePause();
   // }
    
    ImGui::Text("Entities and Components:");
    for (EntityId entity : entities) {
        std::string entityName = "Entity " + std::to_string(entity);
        if (ImGui::TreeNodeEx(entityName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            // Display Position
            if (positions.count(entity)) {
                ImGui::Text("Position: (%.1f, %.1f)", positions.at(entity).position.x, positions.at(entity).position.y);
            }

            // ⭐ New: Display Velocity
            if (velocities.count(entity)) {
                ImGui::Text("Velocity: (%.1f, %.1f)", velocities.at(entity).velocity.x, velocities.at(entity).velocity.y);
            }

            // ⭐ New: Display Shape Type and Size
            //if (shapes.count(entity)) {
              //  const auto& shapeComponent = shapes.at(entity);
              //  ImGui::Text("Shape Type: %s", shapeComponent.type == ShapeComponent::Type::Rectangle ? "Rectangle" : "Circle");
              //  ImGui::Text("Size: (%.1f, %.1f)", shapeComponent.size.x, shapeComponent.size.y);
          //  }
            
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
