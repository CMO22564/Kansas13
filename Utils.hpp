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

// **NOTE: This requires a utility to handle case conversion for full robustness.**

// Helper function to convert a string to an SFML color
inline sf::Color colorFromString(const std::string& colorString) {
    // If you don't have a toLower helper, you MUST match the case used in the JSON (e.g., "Cyan")
    
    if (colorString == "Red" || colorString == "red") {
        return sf::Color::Red;
    }
    if (colorString == "Blue" || colorString == "blue") {
        return sf::Color::Blue;
    }
    if (colorString == "Green" || colorString == "green") {
        return sf::Color::Green;
    }
    if (colorString == "Cyan" || colorString == "cyan") {
        return sf::Color::Cyan;
    }
    if (colorString == "Yellow" || colorString == "yellow") {
        return sf::Color::Yellow;
    }
    if (colorString == "Black" || colorString == "black") {
        return sf::Color::Black;
    }
    if (colorString == "Magenta" || colorString == "magenta") {
        return sf::Color::Magenta;
    }
    
    // If the color string is not found, return White
    return sf::Color::White; 
}
