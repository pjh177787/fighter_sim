#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <map>
#include <string>
#include "base.h"
#include "fighter.h"
#include "map.h"

// Function to print debug messages
void printDebugMessage(const std::string &message);

// Function to check if a location is within the map boundaries
bool isWithinBounds(const std::pair<int, int>& loc, int numRows, int numCols);

// Function to calculate Manhattan distance between two points
int calculateDistanceL1(const std::pair<int, int> &start, const std::pair<int, int> &end);

// Function to calculate distance between two points with BFS
int calculateDistanceBFS(const std::pair<int, int> &start, const std::pair<int, int> &end, const std::vector<Base *> &redBases, int numRows, int numCols);

// Function to order bases by distance from a given fighter
std::vector<Base *> orderBaseDistance(Fighter &fighter, std::vector<Base *> &bases);

// Function to find and assign blue base targets for fighters
void findBlueBase(Map &gameMap, std::vector<Fighter> &fighters, std::vector<Base*>& fuelBlueBases, std::vector<Base*>& missileBlueBases, std::vector<Base *> &redBases, std::map<Fighter *, Base *> &fighterTargets, std::vector<std::string> &commands);

// Function to find and assign red base targets for fighters
void findRedBase(Map &gameMap, std::vector<Fighter> &fighters, std::vector<Base *> &redBases, std::map<Fighter *, Base *> &fighterTargets, std::vector<std::string> &commands);

// Function to validate and reset fighter targets if their target bases are no longer active
void validateAndResetTargets(std::vector<Fighter>& fighters, std::vector<Base*>& activeRedBases, std::map<Fighter*, Base*>& fighterTargets);

// Function to move fighters towards their target base
std::string moveTowardsTarget(Fighter &fighter, const std::pair<int, int> &target, const std::vector<Base*> &redBases);

// Function to move fighters towards their target base
std::string moveTowardsTargetDumb(Fighter &fighter, const std::pair<int, int> &target, const std::vector<Base*> &redBases);

// Function to generate move commands for fighters
void generateMoveCommands(const Map &gameMap, const std::map<Fighter *, Base *> &fighterTargets, std::vector<std::string> &commands, std::vector<Base*> &redBases);

#endif // UTILS_H
