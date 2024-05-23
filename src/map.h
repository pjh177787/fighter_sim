#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <utility>
#include <map>
#include "base.h"

class Map {
public:
    // Method to get the singleton instance of the map
    static Map& getInstance(int rows = 0, int cols = 0, const std::vector<std::string>& mapData = {});

    // Delete copy constructor and assignment operator to prevent copies of the singleton
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    // Get the dimensions of the map
    int getRows() const;
    int getCols() const;

    // Get the character at a specific location
    char getCell(int row, int col) const;

    // Check if a location is within the map boundaries
    bool isValidLocation(int row, int col) const;

    // Check if a location is a red base
    bool isRedBase(int row, int col) const;

    // Check if a location is a blue base
    bool isBlueBase(int row, int col) const;

    // Check if a location is a neutral zone
    bool isNeutralZone(int row, int col) const;

    // Find all red bases on the map
    std::vector<std::pair<int, int>> getRedBases() const;

    // Find all blue bases on the map
    std::vector<std::pair<int, int>> getBlueBases() const;

    // Get the base at a specific location
    Base* getBaseAtLocation(int row, int col) const;

    // Set the base at a specific location
    void setBaseAtLocation(int row, int col, Base& base);

private:
    // Private constructor to prevent direct instantiation
    Map(int rows, int cols, const std::vector<std::string>& mapData);

    int rows;
    int cols;
    std::vector<std::string> mapData;
    std::map<std::pair<int, int>, Base> bases; // Map to store bases with their locations as keys
};

#endif // MAP_H
