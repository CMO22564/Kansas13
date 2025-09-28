#pragma once

#include "Core.hpp" // For RenderComponent and other types
#include <string>
#include <SFML/Graphics/Color.hpp>

// Helper function to convert a string to a shape type
inline RenderComponent::Type shapeTypeFromString(const std::string& typeString) {
    if (typeString == "Square") {
        return RenderComponent::Type::Square;
    }
    if (typeString == "Circle") {
        return RenderComponent::Type::Circle;
    }
    if (typeString == "Triangle") {
        return RenderComponent::Type::Triangle;
    }
    return RenderComponent::Type::Circle; // Default to Circle
}

// Helper function to convert a string to an SFML color
inline sf::Color colorFromString(const std::string& colorString) {
    if (colorString == "Red") {
        return sf::Color::Red;
    }
    if (colorString == "Blue") {
        return sf::Color::Blue;
    }
    if (colorString == "Green") {
        return sf::Color::Green;
    }
    return sf::Color::White; // Default to White
}
