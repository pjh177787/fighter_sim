#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <queue>
#include <utility>
#include <unordered_set>
#include <algorithm>
#include <iomanip>
#include <set>

#include "map.h"
#include "base.h"
#include "fighter.h"

#define INT_MAX 2147483647

using namespace std;
// #define DEBUG

// Function to check if a location is within the map boundaries
bool isWithinBounds(const pair<int, int>& loc, int numRows, int numCols) {
    return loc.first >= 0 && loc.first < numRows && loc.second >= 0 && loc.second < numCols;
}

// Function to calculate Manhattan distance between two points
int calculateDistanceL1(const std::pair<int, int>& start, const std::pair<int, int>& end) {
    return abs(start.first - end.first) + abs(start.second - end.second);
}

// Function to calculate the actual distance between two points using BFS
int calculateDistanceBFS(const std::pair<int, int> &start, const std::pair<int, int> &end, const std::vector<Base *> &redBases, int numRows, int numCols)
{
    if (start == end) return 0;

    auto isActiveRedBase = [&](const std::pair<int, int>& loc) {
        for (const auto& base : redBases) {
            if (base->isActive() && base->getLocation() == loc) {
                return true;
            }
        }
        return false;
    };

    std::queue<std::pair<int, int>> q;
    std::unordered_set<int> visited;
    q.push(start);
    visited.insert(start.first * numCols + start.second);

    std::vector<std::pair<int, int>> directions = {
        { -1, 0 }, // Up
        { 1, 0 },  // Down
        { 0, -1 }, // Left
        { 0, 1 }   // Right
    };

    int distance = 0;

    while (!q.empty()) {
        int levelSize = q.size();
        distance++;
        for (int i = 0; i < levelSize; ++i) {
            auto current = q.front();
            q.pop();

            for (const auto& dir : directions) {
                std::pair<int, int> next = { current.first + dir.first, current.second + dir.second };

                if (next == end) {
                    return distance;
                }

                if (isWithinBounds(next, numRows, numCols) && !isActiveRedBase(next)) {
                    int nextIndex = next.first * numCols + next.second;
                    if (visited.find(nextIndex) == visited.end()) {
                        visited.insert(nextIndex);
                        q.push(next);
                    }
                }
            }
        }
    }

    return -1; // Return -1 if there is no valid path
}

// Function to order bases by distance from a given fighter
std::vector<Base *> orderBaseDistance(Fighter &fighter, std::vector<Base *> &bases, const std::vector<Base *> &redBases, int numRows, int numCols)
{
    std::vector<Base *> orderedBases = bases;

    // Sort the pointers based on their distance to the fighter's location
    std::sort(orderedBases.begin(), orderedBases.end(), [&fighter](Base *a, Base *b)
    {
        int distanceA = calculateDistanceL1(fighter.getLocation(), a->getLocation());
        int distanceB = calculateDistanceL1(fighter.getLocation(), b->getLocation());
        return distanceA < distanceB;
    });

    // Sort the pointers based on their distance to the fighter's location with BFS for the first 3 bases
    // std::sort(orderedBases.begin(), orderedBases.begin()+3, [&fighter, &redBases, numRows, numCols](Base *a, Base *b)
    // {
    //     int distanceA = calculateDistanceBFS(fighter.getLocation(), a->getLocation(), redBases, numRows, numCols);
    //     int distanceB = calculateDistanceBFS(fighter.getLocation(), b->getLocation(), redBases, numRows, numCols);
    //     return distanceA < distanceB;
    // });

    return orderedBases;
}

void findBlueBase(Map &gameMap, vector<Fighter> &fighters, vector<Base*>& fuelBlueBases, vector<Base*>& missileBlueBases, vector<Base*>& redBases, map<Fighter *, Base *> &fighterTargets, vector<string> &commands)
{
    for (auto &fighter : fighters)
    {
        bool needMissiles = fighter.getCurrentMissiles() == 0;
        bool isAtBase = false;

        // Create a set to hold the unique bases
        set<Base*> togetherBaseSet;
        togetherBaseSet.insert(missileBlueBases.begin(), missileBlueBases.end());
        togetherBaseSet.insert(fuelBlueBases.begin(), fuelBlueBases.end());

        // Check if the fighter is already in a base
        for (auto &base : togetherBaseSet)
        {
            if (fighter.getLocation() == base->getLocation())
            {
                isAtBase = true;
                fighterTargets[&fighter] = nullptr; // Reset the target for this fighter

                if (base->getFuelReserve() > 0)
                {
                    int fuelNeeded = fighter.getFuelCapacity() - fighter.getCurrentFuel();
                    int fuelProvided = min(fuelNeeded, base->getFuelReserve());
                    base->setFuelReserve(base->getFuelReserve() - fuelProvided);
                    fighter.replenishFuel(fuelProvided);
                    commands.push_back("fuel " + to_string(fighter.getId()) + " " + to_string(fuelProvided));
                }

                if (base->getMissileReserve() > 0)
                {
                    int missilesNeeded = fighter.getMaxMissileCarry() - fighter.getCurrentMissiles();
                    int missilesProvided = min(missilesNeeded, base->getMissileReserve());
                    base->setMissileReserve(base->getMissileReserve() - missilesProvided);
                    fighter.replenishMissiles(missilesProvided);
                    commands.push_back("missile " + to_string(fighter.getId()) + " " + to_string(missilesProvided));
                }

                break;
            }
        }

        if (!isAtBase)
        {
            // Order blue bases by distance from the fighter
            auto orderedMissileBases = orderBaseDistance(fighter, missileBlueBases, redBases, gameMap.getRows(), gameMap.getCols());
            // Match the fighter with the nearest base with at least some missiles
            if (needMissiles) {
                for (auto &base : orderedMissileBases)
                {
                    if (base->getMissileReserve() > 0)
                    {
                        fighterTargets[&fighter] = base;
                        break;
                    }
                }
                break;
            }
            // Order blue bases by distance from the fighter
            auto orderedFuelBases = orderBaseDistance(fighter, fuelBlueBases, redBases, gameMap.getRows(), gameMap.getCols());
            // Match the fighter with the nearest base with at least some fuel

            // Check if there are more than 3 fuel bases
            int checkBase = orderedFuelBases[2] ? 2:0;

            int roughDistanceToNearestBase = orderedFuelBases[checkBase] ? calculateDistanceL1(fighter.getLocation(), orderedFuelBases[checkBase]->getLocation()) : INT_MAX;
            if (fighter.getCurrentFuel() < roughDistanceToNearestBase + 10) // fuel is short
            {
                for (auto &base : orderedFuelBases)
                {
                    if (base->getFuelReserve() > 0)
                    {
                        // Check the condition for refuel: remaining fuel is not more than what it needs to reach the nearest blue base
                        int distanceToNearestBase = base ? calculateDistanceBFS(fighter.getLocation(), base->getLocation(), redBases, gameMap.getRows(), gameMap.getCols()) : INT_MAX;
                        if (fighter.getCurrentFuel() <= distanceToNearestBase + 1)
                        {
                            fighterTargets[&fighter] = base;
                            break;
                        }
                    }
                }
            }

            // Check if the fighter is directly next to a fuel blue base
            auto nearestBase = orderedFuelBases.size() > 0 ? orderedFuelBases[0] : nullptr;
            if (calculateDistanceL1(fighter.getLocation(), nearestBase->getLocation()) == 1 && fighter.getCurrentFuel() < 10 && nearestBase->getFuelReserve() > 0)
            {
                fighterTargets[&fighter] = nearestBase;
                break;
            }
        }
    }
}

void findRedBase(Map &gameMap, vector<Fighter> &fighters, vector<Base *> &redBases, map<Fighter *, Base *> &fighterTargets, vector<string> &commands)
{
    for (auto &fighter : fighters)
    {
        bool isNextToRedBase = false;

        // Check if the fighter's target is a blue base
        if (fighterTargets[&fighter] != nullptr && fighterTargets[&fighter]->getTeam() == "blue")
        {
            #ifdef DEBUG
            cout << "Fighter " << fighter.getId() << " is returning to blue base" << endl;
            #endif
            continue; // Skip if the fighter is returning to a blue base
        }

        // Check if the fighter has missile to attack a red base
        if (fighter.getCurrentMissiles() == 0)
        {
            #ifdef DEBUG
            cout << "Fighter " << fighter.getId() << " has no missiles" << endl;
            #endif
            continue; // Skip if the fighter has no missiles
        }


        // Check if the fighter is already next to a red base
        for (auto &base : redBases)
        {
            int distance = calculateDistanceL1(fighter.getLocation(), base->getLocation());
            if (distance == 1 && base->isActive())
            {
                int missilesToLaunch = min(fighter.getCurrentMissiles(), base->getHitPoints());

                // Determine the attack direction based on the relative position
                pair<int, int> fighterLoc = fighter.getLocation();
                pair<int, int> baseLoc = base->getLocation();
                int direction = -1;

                if (fighterLoc.first > baseLoc.first)
                {
                    direction = 0; // up
                }
                else if (fighterLoc.first < baseLoc.first)
                {
                    direction = 1; // down
                }
                else if (fighterLoc.second > baseLoc.second)
                {
                    direction = 2; // left
                }
                else if (fighterLoc.second < baseLoc.second)
                {
                    direction = 3; // right
                }

                if (direction != -1)
                {
                    commands.push_back("attack " + to_string(fighter.getId()) + " " + to_string(direction) + " " + to_string(missilesToLaunch));
                    fighter.attack(direction, missilesToLaunch); // Update the fighter's missile count
                    base->takeDamage(missilesToLaunch);          // Update the base's hit points
                    if (base->getHitPoints() == 0 || fighter.getCurrentMissiles() == 0)
                    {
                        fighterTargets[&fighter] = nullptr; // Reset the target for this fighter
                    }
                }
                isNextToRedBase = true;
                break;
            }
        }

        if (!isNextToRedBase && fighterTargets[&fighter] == nullptr)
        {
            // If the fighter is not next to a red base and does not have a target
            auto orderedRedBases = orderBaseDistance(fighter, redBases, redBases, gameMap.getRows(), gameMap.getCols());

            for (auto &redBase : orderedRedBases)
            {
                if (redBase->isActive())
                {
                    fighterTargets[&fighter] = redBase;
                    break;
                }
            }
        }
    }
}

void validateAndResetTargets(vector<Fighter> &fighters, vector<Base *> &activeRedBases, map<Fighter *, Base *> &fighterTargets)
{
    for (auto &fighter : fighters)
    {
        Base *target = fighterTargets[&fighter];
        if (target != nullptr)
        {
            // Check if the target is still active
            bool isActive = false;
            for (auto &base : activeRedBases)
            {
                if (base == target)
                {
                    isActive = true;
                    break;
                }
            }

            // If the target is not active, reset the target
            if (!isActive)
            {
                fighterTargets[&fighter] = nullptr;
            }
        }
    }
}

string moveTowardsTargetDumb(Fighter &fighter, const pair<int, int> &target, const vector<Base *> &redBases, int numRows, int numCols)
{
    // Check if the next location is an active red base
    auto isActiveRedBase = [&](const pair<int, int> &loc)
    {
        for (const auto &base : redBases)
        {
            if (base->isActive() && base->getLocation() == loc)
            {
                return true;
            }
        }
        return false;
    };

    // Check if the next location is within the map boundaries
    auto isValidMove = [&](const pair<int, int> &loc)
    {
        return isWithinBounds(loc, numRows, numCols) && !isActiveRedBase(loc);
    };

    pair<int, int> currentLocation = fighter.getLocation();
    pair<int, int> nextLocation;
    string moveCommand;

    // Determine possible move directions and their next locations
    vector<pair<int, pair<int, int>>> directions = {
        {0, {currentLocation.first - 1, currentLocation.second}}, // Up
        {1, {currentLocation.first + 1, currentLocation.second}}, // Down
        {2, {currentLocation.first, currentLocation.second - 1}}, // Left
        {3, {currentLocation.first, currentLocation.second + 1}}  // Right
    };

    // Primary direction based on the target's location
    if (abs(currentLocation.first - target.first) > abs(currentLocation.second - target.second)) // Move vertically first
    {
        #ifdef DEBUG
        cout << "Moving vertically first" << endl;
        cout << "Current Location: (" << currentLocation.first << ", " << currentLocation.second << ")" << endl;
        cout << "Target Location: (" << target.first << ", " << target.second << ")" << endl;
        #endif
        if (currentLocation.first > target.first)
        {
            nextLocation = directions[0].second;
            if (isValidMove(nextLocation))
            {
                moveCommand = "move " + to_string(fighter.getId()) + " 0"; // Up
                fighter.moveUp();
                return moveCommand;
            }
            else // If the next location is not valid, move horizontally
            {
                if (currentLocation.second > target.second)
                {
                    nextLocation = directions[2].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 2"; // Left
                        fighter.moveLeft();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[3].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 3"; // Right
                        fighter.moveRight();
                        return moveCommand;
                    }
                }
            }
        }
        else if (currentLocation.first < target.first)
        {
            nextLocation = directions[1].second;
            if (isValidMove(nextLocation))
            {
                moveCommand = "move " + to_string(fighter.getId()) + " 1"; // Down
                fighter.moveDown();
                return moveCommand;
            }
            else // If the next location is not valid, move horizontally
            {
                if (currentLocation.second > target.second)
                {
                    nextLocation = directions[2].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 2"; // Left
                        fighter.moveLeft();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[3].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 3"; // Right
                        fighter.moveRight();
                        return moveCommand;
                    }
                }
            }
        }
    }
    else // Move horizontally first
    {
        #ifdef DEBUG
        cout << "Moving horizontally first" << endl;
        cout << "Current Location: (" << currentLocation.first << ", " << currentLocation.second << ")" << endl;
        cout << "Target Location: (" << target.first << ", " << target.second << ")" << endl;
        #endif
        if (currentLocation.second > target.second)
        {
            nextLocation = directions[2].second;
            if (isValidMove(nextLocation))
            {
                moveCommand = "move " + to_string(fighter.getId()) + " 2"; // Left
                fighter.moveLeft();
                return moveCommand;
            }
            else
            { // If the next location is not valid, move vertically
                if (currentLocation.first > target.first)
                {
                    nextLocation = directions[0].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 0"; // Up
                        fighter.moveUp();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[1].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 1"; // Down
                        fighter.moveDown();
                        return moveCommand;
                    }
                }
            }
        }
        else if (currentLocation.second < target.second)
        {
            nextLocation = directions[3].second;
            if (isValidMove(nextLocation))
            {
                moveCommand = "move " + to_string(fighter.getId()) + " 3"; // Right
                fighter.moveRight();
                return moveCommand;
            }
            else
            { // If the next location is not valid, move vertically
                if (currentLocation.first > target.first)
                {
                    nextLocation = directions[0].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 0"; // Up
                        fighter.moveUp();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[1].second;
                    if (isValidMove(nextLocation))
                    {
                        moveCommand = "move " + to_string(fighter.getId()) + " 1"; // Down
                        fighter.moveDown();
                        return moveCommand;
                    }
                }
            }
        }
    }
    return ""; // No valid move if all possible moves are blocked by active red bases or out of bounds
}

string moveTowardsTarget(Fighter &fighter, const pair<int, int> &target, const vector<Base *> &redBases, int numRows, int numCols)
{
    auto currentLocation = fighter.getLocation();
    if (currentLocation == target) return "";

    // Check if the next location is an active red base
    auto isActiveRedBase = [&](const std::pair<int, int>& loc) {
        for (const auto& base : redBases) {
            if (base->isActive() && base->getLocation() == loc) {
                return true;
            }
        }
        return false;
    };

    if (calculateDistanceL1(currentLocation, target) <= 2) {
        return moveTowardsTargetDumb(fighter, target, redBases, numRows, numCols);
    }

    std::queue<std::pair<int, int>> q;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    q.push(currentLocation);
    parent[currentLocation] = currentLocation; // Mark the start node with itself

    std::vector<std::pair<int, int>> directions = {
        { -1, 0 }, // Up
        { 1, 0 },  // Down
        { 0, -1 }, // Left
        { 0, 1 }   // Right
    };

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        for (const auto& dir : directions) {
            std::pair<int, int> next = { current.first + dir.first, current.second + dir.second };

            if (next == target) {
                // Trace back to find the first step
                while (parent[current] != currentLocation) {
                    current = parent[current];
                }
                // Determine the move direction
                if (current.first < currentLocation.first) {
                    fighter.moveUp();
                    return "move " + std::to_string(fighter.getId()) + " 0"; // Up
                } else if (current.first > currentLocation.first) {
                    fighter.moveDown();
                    return "move " + std::to_string(fighter.getId()) + " 1"; // Down
                } else if (current.second < currentLocation.second) {
                    fighter.moveLeft();
                    return "move " + std::to_string(fighter.getId()) + " 2"; // Left
                } else if (current.second > currentLocation.second) {
                    fighter.moveRight();
                    return "move " + std::to_string(fighter.getId()) + " 3"; // Right
                }
            }

            if (isWithinBounds(next, numRows, numCols) && !isActiveRedBase(next) && parent.find(next) == parent.end()) {
                parent[next] = current;
                q.push(next);
            }
        }
    }

    return ""; 
    // No valid move if all possible moves are blocked by active red bases or out of bounds
}


void generateMoveCommands(const Map &gameMap, const map<Fighter *, Base *> &fighterTargets, vector<string> &commands, vector<Base *> &redBases)
{
    for (const auto &[fighter, targetBase] : fighterTargets)
    {
        if (targetBase == nullptr)
        {
            continue; // Skip if the target is nullptr
        }

        // Generate the move command for the fighter to move towards the target base
        string moveCommand = moveTowardsTarget(*fighter, targetBase->getLocation(), redBases, gameMap.getRows(), gameMap.getCols());
        if (!moveCommand.empty())
        {
            commands.push_back(moveCommand);
        }
    }
}
