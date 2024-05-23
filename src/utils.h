#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <map>
#include <string>
#include "base.h"
#include "fighter.h"
#include "map.h"

// Function to calculate Manhattan distance between two points
int calculateDistance(const std::pair<int, int> &loc1, const std::pair<int, int> &loc2);

// Function to order bases by distance from a given fighter
std::vector<Base *> orderBaseDistance(Fighter &fighter, std::vector<Base *> &bases);

// Function to find and assign blue base targets for fighters
void findBlueBase(std::vector<Fighter> &fighters, std::vector<Base *> &blueBases, std::map<Fighter *, Base *> &fighterTargets, std::vector<std::string> &commands);

// Function to find and assign red base targets for fighters
void findRedBase(std::vector<Fighter> &fighters, std::vector<Base *> &redBases, std::map<Fighter *, Base *> &fighterTargets, std::vector<std::string> &commands);

// Function to validate and reset fighter targets if their target bases are no longer active
void validateAndResetTargets(std::vector<Fighter>& fighters, std::vector<Base*>& activeRedBases, std::map<Fighter*, Base*>& fighterTargets);

// Function to move fighters towards their target base
std::string moveTowardsTarget(Fighter &fighter, const std::pair<int, int> &target, const std::vector<Base*> &redBases);

// Function to generate move commands for fighters
void generateMoveCommands(const std::map<Fighter *, Base *> &fighterTargets, std::vector<std::string> &commands, std::vector<Base*> &redBases);

// Function to pretty print the map, information of all the bases, and information of all the fighters
void debugPrint(const Map& gameMap, const std::vector<Base*>& blueBases, const std::vector<Base*>& redBases, const std::vector<Fighter>& fighters, const std::map<Fighter*, Base*>& fighterTargets);

#endif // UTILS_H
