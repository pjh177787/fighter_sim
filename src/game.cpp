#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "map.h"
#include "base.h"
#include "fighter.h"
#include "utils.h"

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
