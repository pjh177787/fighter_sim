#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>
#include "map.h"
#include "base.h"
#include "fighter.h"

#define INT_MAX 2147483647
// #define DEBUG

using namespace std;

string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first);
}

// Function to calculate Manhattan distance between two points
int calculateDistance(const pair<int, int> &loc1, const pair<int, int> &loc2)
{
    return abs(loc1.first - loc2.first) + abs(loc1.second - loc2.second);
}

// Function to order bases by distance from a given fighter
vector<Base *> orderBaseDistance(Fighter &fighter, vector<Base *> &bases)
{
    vector<Base *> orderedBases = bases;

    // Sort the pointers based on their distance to the fighter's location
    sort(orderedBases.begin(), orderedBases.end(), [&fighter](Base *a, Base *b)
         {
        int distanceA = calculateDistance(fighter.getLocation(), a->getLocation());
        int distanceB = calculateDistance(fighter.getLocation(), b->getLocation());
        return distanceA < distanceB; });

    return orderedBases;
}

void findBlueBase(vector<Fighter> &fighters, vector<Base *> &blueBases, map<Fighter *, Base *> &fighterTargets, vector<string> &commands)
{
    for (auto &fighter : fighters)
    {
        bool needMissiles = fighter.getCurrentMissiles() == 0;
        bool isAtBase = false;

        if (!isAtBase)
        {
            // Order blue bases by distance from the fighter
            auto orderedBases = orderBaseDistance(fighter, blueBases);

            // Match the fighter with the nearest base with at least some fuel
            for (auto &base : orderedBases)
            {
                if (base->getFuelReserve() > 0)
                {
                    // Check the condition for refuel: remaining fuel is not more than what it needs to reach the third nearest blue base
                    auto nearestBase = orderedBases.size() > 0 ? orderedBases[0] : nullptr;
                    int distanceToNearestBase = nearestBase ? calculateDistance(fighter.getLocation(), nearestBase->getLocation()) : INT_MAX;
                    if (fighter.getCurrentFuel() <= distanceToNearestBase)
                    {
                        fighterTargets[&fighter] = base;
                    }
                }
            }
        }

        // Check if the fighter is already in a base
        for (auto &base : blueBases)
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

                if (needMissiles && base->getMissileReserve() > 0)
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
    }
}

void findRedBase(vector<Fighter> &fighters, vector<Base *> &redBases, map<Fighter *, Base *> &fighterTargets, vector<string> &commands)
{
    for (auto &fighter : fighters)
    {
        bool isNextToRedBase = false;

        // Check if the fighter is already next to a red base
        for (auto &base : redBases)
        {
            int distance = calculateDistance(fighter.getLocation(), base->getLocation());
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
                    if (base->getHitPoints() == 0)
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
            auto orderedRedBases = orderBaseDistance(fighter, redBases);

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

string moveTowardsTarget(Fighter &fighter, const pair<int, int> &target, const vector<Base *> &redBases)
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
        if (currentLocation.first > target.first)
        {
            nextLocation = directions[0].second;
            if (!isActiveRedBase(nextLocation))
            {
                moveCommand = "move " + to_string(fighter.getId()) + " 0"; // Up
                fighter.moveUp();
                return moveCommand;
            }
            else // If the next location is an active red base, move horizontally
            {
                if (currentLocation.second > target.second)
                {
                    nextLocation = directions[2].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 2"; // Left
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveLeft();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[3].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 3"; // Right
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveRight();
                        return moveCommand;
                    }
                }
            }
        }
        else if (currentLocation.first < target.first)
        {
            nextLocation = directions[1].second;
            moveCommand = "move " + to_string(fighter.getId()) + " 1"; // Down
            if (!isActiveRedBase(nextLocation))
            {
                fighter.moveDown();
                return moveCommand;
            }
            else // If the next location is an active red base, move horizontally
            {
                if (currentLocation.second > target.second)
                {
                    nextLocation = directions[2].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 2"; // Left
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveLeft();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[3].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 3"; // Right
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveRight();
                        return moveCommand;
                    }
                }
            }
        }
    }
    else // Move horizontally first
    {
        if (currentLocation.second > target.second)
        {
            nextLocation = directions[2].second;
            moveCommand = "move " + to_string(fighter.getId()) + " 2"; // Left
            if (!isActiveRedBase(nextLocation))
            {
                fighter.moveLeft();
                return moveCommand;
            }
            else
            { // If the next location is an active red base, move vertically
                if (currentLocation.first > target.first)
                {
                    nextLocation = directions[0].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 0"; // Up
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveUp();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[1].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 1"; // Down
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveDown();
                        return moveCommand;
                    }
                }
            }
        }
        else if (currentLocation.second < target.second)
        {
            nextLocation = directions[3].second;
            moveCommand = "move " + to_string(fighter.getId()) + " 3"; // Right
            if (!isActiveRedBase(nextLocation))
            {
                fighter.moveRight();
                return moveCommand;
            }
            else
            { // If the next location is an active red base, move vertically
                if (currentLocation.first > target.first)
                {
                    nextLocation = directions[0].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 0"; // Up
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveUp();
                        return moveCommand;
                    }
                }
                else
                {
                    nextLocation = directions[1].second;
                    moveCommand = "move " + to_string(fighter.getId()) + " 1"; // Down
                    if (!isActiveRedBase(nextLocation))
                    {
                        fighter.moveDown();
                        return moveCommand;
                    }
                }
            }
        }
    }
    return ""; // No valid move if all possible moves are blocked by active red bases
}

void generateMoveCommands(const map<Fighter *, Base *> &fighterTargets, vector<string> &commands, vector<Base *> &redBases)
{
    for (const auto &[fighter, targetBase] : fighterTargets)
    {
        if (targetBase == nullptr)
        {
            continue; // Skip if the target is nullptr
        }

        // Generate the move command for the fighter to move towards the target base
        string moveCommand = moveTowardsTarget(*fighter, targetBase->getLocation(), redBases);
        if (!moveCommand.empty())
        {
            commands.push_back(moveCommand);
        }
    }
}

void debugPrint(Map &gameMap, vector<Base *> &blueBases, vector<Base *> &redBases, vector<Fighter> &fighters, map<Fighter *, Base *> &fighterTargets)
{
    // Create two maps for printing
    vector<string> baseMap(gameMap.getRows(), string(gameMap.getCols(), '.'));
    vector<string> fighterMap(gameMap.getRows(), string(gameMap.getCols(), '.'));

    // Populate the baseMap with blue and red bases
    for (auto &base : blueBases)
    {
        auto [x, y] = base->getLocation();
        baseMap[x][y] = '*'; // Blue base
    }
    for (auto &base : redBases)
    {
        auto [x, y] = base->getLocation();
        baseMap[x][y] = '#'; // Red base
    }

    // Populate the fighterMap with fighter locations
    for (auto &fighter : fighters)
    {
        auto [x, y] = fighter.getLocation();
        fighterMap[x][y] = '+'; // Fighter
    }

    // Print the two maps side by side
    cout << "Bases Map:              Fighters Map:" << endl;
    for (int i = 0; i < gameMap.getRows(); ++i)
    {
        cout << baseMap[i] << "               " << fighterMap[i] << endl;
    }
    cout << endl;

    // Print information of all blue bases
    cout << "Blue Bases:" << endl;
    for (const auto &base : blueBases)
    {
        cout << "Location: (" << base->getLocation().first << ", " << base->getLocation().second << ")"
             << ", Fuel Reserve: " << base->getFuelReserve()
             << ", Missile Reserve: " << base->getMissileReserve()
             << ", Hit Points: " << base->getHitPoints()
             << ", Military Value: " << base->getMilitaryValue()
             << ", Active: " << (base->isActive() ? "Yes" : "No") << endl;
    }
    cout << endl;

    // Print information of all red bases
    cout << "Red Bases:" << endl;
    for (const auto &base : redBases)
    {
        cout << "Location: (" << base->getLocation().first << ", " << base->getLocation().second << ")"
             << ", Fuel Reserve: " << base->getFuelReserve()
             << ", Missile Reserve: " << base->getMissileReserve()
             << ", Hit Points: " << base->getHitPoints()
             << ", Military Value: " << base->getMilitaryValue()
             << ", Active: " << (base->isActive() ? "Yes" : "No") << endl;
    }
    cout << endl;

    // Print information of all fighters
    cout << "Fighters:" << endl;
    for (auto &fighter : fighters)
    {
        cout << "ID: " << fighter.getId()
             << ", Location: (" << fighter.getLocation().first << ", " << fighter.getLocation().second << ")"
             << ", Fuel Capacity: " << fighter.getFuelCapacity()
             << ", Current Fuel: " << fighter.getCurrentFuel()
             << ", Max Missile Carry: " << fighter.getMaxMissileCarry()
             << ", Current Missiles: " << fighter.getCurrentMissiles()
             << ", Target: ";
        if (fighterTargets[&fighter] == nullptr)
        {
            cout << "none" << endl;
        }
        else
        {
            auto targetLoc = fighterTargets[&fighter]->getLocation();
            cout << "(" << targetLoc.first << ", " << targetLoc.second << ")" << endl;
        }
    }
    cout << endl;
}

int main()
{
    int maxFrame = 15000;
    string firstLine;
    getline(cin, firstLine);
    istringstream iss(firstLine);

    int n, m;
    iss >> n >> m;

    // Read the map data
    vector<string> mapData(n);
    for (int i = 0; i < n; ++i)
    {
        getline(cin, mapData[i]);
        mapData[i] = trim(mapData[i]);
    }

    // Initialize the map
    Map &gameMap = Map::getInstance(n, m, mapData);

    // Vectors for bases
    vector<Base *> blueBases;
    vector<Base *> redBases;

    // Read blue bases
    int blueBaseCount;
    cin >> blueBaseCount;
    cin.ignore(); // Ignore the newline character after reading an integer

    for (int i = 0; i < blueBaseCount; ++i)
    {
        int x, y, fuelReserve, missileReserve, hitPoints, militaryValue;
        cin >> x >> y >> fuelReserve >> missileReserve >> hitPoints >> militaryValue;
        cin.ignore(); // Ignore the newline character after reading each base's details
        Base *blueBase = new Base(make_pair(x, y), fuelReserve, missileReserve, hitPoints, militaryValue, "blue");
        // Add blue base to the map and vector
        gameMap.setBaseAtLocation(x, y, *blueBase);
        blueBases.push_back(blueBase);
    }

    // Read red bases
    int redBaseCount;
    cin >> redBaseCount;
    cin.ignore(); // Ignore the newline character after reading an integer

    for (int i = 0; i < redBaseCount; ++i)
    {
        int x, y, fuelReserve, missileReserve, hitPoints, militaryValue;
        cin >> x >> y >> fuelReserve >> missileReserve >> hitPoints >> militaryValue;
        cin.ignore(); // Ignore the newline character after reading each base's details
        Base *redBase = new Base(make_pair(x, y), fuelReserve, missileReserve, hitPoints, militaryValue, "red");
        // Add red base to the map and vector
        gameMap.setBaseAtLocation(x, y, *redBase);
        redBases.push_back(redBase);
    }

    // Read fighters
    int fighterCount;
    cin >> fighterCount;
    cin.ignore(); // Ignore the newline character after reading an integer

    vector<Fighter> fighters;
    for (int i = 0; i < fighterCount; ++i)
    {
        int x, y, fuelCapacity, maxMissileCarry;
        cin >> x >> y >> fuelCapacity >> maxMissileCarry;
        cin.ignore(); // Ignore the newline character after reading each fighter's details
        Fighter fighter(i, make_pair(x, y), fuelCapacity, maxMissileCarry);
        fighters.push_back(fighter);
    }

    // Main game loop
    map<Fighter *, Base *> fighterTargets;
    for (int frame = 0; frame < maxFrame; ++frame)
    {

        vector<string> commands;

        vector<Base *> activeRedBases;
        for (auto &base : redBases)
        {
            if (base->isActive())
            {
                activeRedBases.push_back(base);
            }
        }

        // Check if there are no active red bases left
        if (activeRedBases.empty())
        {
            cout << "OK" << endl;
            continue; // Skip to the next frame
        }

        vector<Base *> resourcefulBlueBases;
        for (auto &base : blueBases)
        {
            if (base->getFuelReserve() > 0 || base->getMissileReserve() > 0)
            {
                resourcefulBlueBases.push_back(base);
            }
        }
#ifdef DEBUG
        maxFrame = 15;
        cout << endl;
        cout << "Frame: " << frame << endl;
        debugPrint(gameMap, resourcefulBlueBases, activeRedBases, fighters, fighterTargets);
#endif

        // Validate and reset targets if necessary
        validateAndResetTargets(fighters, activeRedBases, fighterTargets);

        // Step 1: Establish the mapping of fighters to blue bases
        findBlueBase(fighters, resourcefulBlueBases, fighterTargets, commands);

        // Step 2: Map fighters with enough fuel and missiles to red bases
        findRedBase(fighters, activeRedBases, fighterTargets, commands);

        // Step 3: Generate move commands for fighters not at a base or next to a red base
        generateMoveCommands(fighterTargets, commands, activeRedBases);

        // Print commands for this frame
        for (const auto &command : commands)
        {
            cout << command << endl;
        }

        // End of frame
        cout << "OK" << endl;
    }

    // Clean up dynamically allocated bases
    for (auto base : blueBases)
    {
        delete base;
    }
    for (auto base : redBases)
    {
        delete base;
    }

    return 0;
}
