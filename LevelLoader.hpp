// LevelLoader.hpp
#pragma once

#include <vector>
#include <string>
#include "LevelData.hpp" // Needs to know what LevelData is

// Declaration of the public loading function
std::vector<LevelData> loadLevelsFromFile(const std::string& filePath);

// NOTE: You do NOT put the struct/class definitions (like EnemyTypeData::from_json) 
// in this header. Only the function declaration.
