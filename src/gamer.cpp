#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "map.h"
#include "base.h"
#include "fighter.h"
#include "utils.h"

#define INT_MAX 2147483647
#define DEBUG 15000 // Set to a frame number to enable debug output for the last 10 frames

using namespace std;

string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first);
}


void debugPrint(Map &gameMap, vector<Base *> &fuelBlueBases, vector<Base *> &missileBlueBases, vector<Base *> &resourcefulBlueBases, vector<Base *> &redBases, vector<Fighter> &fighters, map<Fighter *, Base *> &fighterTargets)
{
    // Create two maps for printing
    vector<string> baseMap(gameMap.getRows(), string(gameMap.getCols(), '.'));
    vector<string> fighterMap(gameMap.getRows(), string(gameMap.getCols(), '.'));

    // Populate the baseMap with blue and red bases
    for (auto &base : fuelBlueBases)
    {
        auto [x, y] = base->getLocation();
        baseMap[x][y] = 'F'; // Blue base
    }
    for (auto &base : missileBlueBases)
    {
        auto [x, y] = base->getLocation();
        baseMap[x][y] = 'M'; // Blue base
    }
    for (auto &base : resourcefulBlueBases)
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
        if (baseMap[x][y] == '*' || baseMap[x][y] == 'F' || baseMap[x][y] == 'M')
            baseMap[x][y] = '@'; // Fighter at blue base
        else
            baseMap[x][y] = '+'; // Fighter not at blue base
    }

    // Print the map with line numbers
    printDebugMessage("Bases Map:");
    for (int i = 0; i < gameMap.getRows(); ++i)
    {
        // print line number, zeropadded to 4 digits
        stringstream ss;
        ss << setw(4) << setfill('0') << i << "   " << baseMap[i];
        printDebugMessage(ss.str());
    }

    // // Print information of all blue bases
    // printDebugMessage("Missile Blue Bases:");
    // for (const auto &base : missileBlueBases)
    // {
    //     stringstream ss;
    //     ss << "Location: (" << base->getLocation().first << ", " << base->getLocation().second << ")"
    //         << ", Fuel Reserve: " << base->getFuelReserve()
    //         << ", Missile Reserve: " << base->getMissileReserve()
    //         << ", Hit Points: " << base->getHitPoints()
    //         << ", Military Value: " << base->getMilitaryValue()
    //         << ", Active: " << (base->isActive() ? "Yes" : "No");
    //     printDebugMessage(ss.str());
    // }
    // printDebugMessage("");

    // // Print information of all red bases
    // printDebugMessage("Red Bases:");
    // for (const auto &base : redBases)
    // {
    //     stringstream ss;
    //     ss << "Location: (" << base->getLocation().first << ", " << base->getLocation().second << ")"
    //         << ", Fuel Reserve: " << base->getFuelReserve()
    //         << ", Missile Reserve: " << base->getMissileReserve()
    //         << ", Hit Points: " << base->getHitPoints()
    //         << ", Military Value: " << base->getMilitaryValue()
    //         << ", Active: " << (base->isActive() ? "Yes" : "No");
    //     printDebugMessage(ss.str());
    // }
    // printDebugMessage("");

    // Print information of all fighters
    printDebugMessage("Fighters:");
    for (auto &fighter : fighters)
    {
        stringstream ss;
        ss << "ID: " << fighter.getId()
           << ", Location: (" << fighter.getLocation().first << ", " << fighter.getLocation().second << ")"
           << ", Fuel Capacity: " << fighter.getFuelCapacity()
           << ", Current Fuel: " << fighter.getCurrentFuel()
           << ", Max Missile Carry: " << fighter.getMaxMissileCarry()
           << ", Current Missiles: " << fighter.getCurrentMissiles()
           << ", Target: ";
        if (fighterTargets[&fighter] == nullptr)
        {
            ss << "none";
        }
        else
        {
            auto targetLoc = fighterTargets[&fighter]->getLocation();
            ss << "(" << targetLoc.first << ", " << targetLoc.second << ")";
            ss << ", Target is " << fighterTargets[&fighter]->getTeam();
        }
        printDebugMessage(ss.str());
    }

    // Print the number of remaining red bases
    stringstream ss;
    ss << "Remaining Red Bases: " << count_if(redBases.begin(), redBases.end(), [](Base *base)
                                              { return base->isActive(); });
    printDebugMessage(ss.str());
    printDebugMessage("");
}

int main()
{


#ifdef DEBUG
    // Clear the debug file
    std::ofstream debugFile("debug.txt");
    debugFile.close();
#endif

    // Read the map dimensions
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
#ifdef DEBUG
    // print most possible fuel and missile, summed up from all blue bases
    int totalFuel = 0;    // Total fuel from all blue bases
    int totalMissile = 0; // Total missile from all blue bases
    for (auto &base : blueBases)
    {
        totalFuel += base->getFuelReserve();
        totalMissile += base->getMissileReserve();
    }
    printDebugMessage("Total fuel: " + to_string(totalFuel));
    printDebugMessage("Total missile: " + to_string(totalMissile));

    // print the total military value and hp of all red bases
    int totalMilitaryValue = 0; // Total military value of all red bases
    int totalHitPoints = 0;     // Total hit points of all red bases
    for (auto &base : redBases)
    {
        totalMilitaryValue += base->getMilitaryValue();
        totalHitPoints += base->getHitPoints();
    }
    printDebugMessage("Total military value: " + to_string(totalMilitaryValue));
    printDebugMessage("Total hit points: " + to_string(totalHitPoints));
#endif

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
            printDebugMessage("OK");
            continue; // Skip to the next frame
        }

        vector<Base *> fuelBlueBases;
        vector<Base *> missileBlueBases;
        vector<Base *> resourcefulBlueBases;
        for (auto &base : blueBases)
        {
            if (base->getFuelReserve() > 0)
            {
                fuelBlueBases.push_back(base);
            }
            if (base->getMissileReserve() > 0)
            {
                missileBlueBases.push_back(base);
            }
            if (base->getFuelReserve() > 0 && base->getMissileReserve() > 0)
            {
                resourcefulBlueBases.push_back(base);
            }
        }
#ifdef DEBUG
        maxFrame = DEBUG;
        printDebugMessage("");
        printDebugMessage("Frame: " + to_string(frame));
        if (frame >= maxFrame - 10)
        {
            debugPrint(gameMap, fuelBlueBases, missileBlueBases, resourcefulBlueBases, activeRedBases, fighters, fighterTargets);
        }
#endif

        // Validate and reset targets if necessary
        validateAndResetTargets(fighters, activeRedBases, fighterTargets);

        // Step 1: Establish the mapping of fighters to blue bases
        findBlueBase(gameMap, fighters, fuelBlueBases, missileBlueBases, redBases, fighterTargets, commands);

        // Step 2: Map fighters with enough fuel and missiles to red bases
        findRedBase(gameMap, fighters, activeRedBases, fighterTargets, commands);

        // Step 3: Generate move commands for fighters not at a base or next to a red base
        generateMoveCommands(gameMap, fighterTargets, commands, activeRedBases);

        // Print commands for this frame
        for (const auto &command : commands)
        {
            cout << command << endl;
            printDebugMessage(command);
        }
#ifdef DEBUG
        if (frame >= maxFrame - 10)
        {
            debugPrint(gameMap, fuelBlueBases, missileBlueBases, resourcefulBlueBases, activeRedBases, fighters, fighterTargets);
        }
#endif

        // End of frame
        cout << "OK" << endl;
        printDebugMessage("OK");
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
