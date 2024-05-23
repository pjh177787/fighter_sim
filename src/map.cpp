#include "map.h"
#include <stdexcept>
#include <iostream>

// Singleton Pattern: The Map class is implemented as a singleton, with a private constructor
// and a static method getInstance to ensure only one instance of the map exists.
// The getInstance method takes optional parameters for the map dimensions and data,
// which are used only the first time the singleton instance is created.
// This ensures the Map class is globally accessible while maintaining a single instance.

Map& Map::getInstance(int rows, int cols, const std::vector<std::string>& mapData) {
    static Map instance(rows, cols, mapData);
    return instance;
}

Map::Map(int rows, int cols, const std::vector<std::string>& mapData)
    : rows(rows), cols(cols), mapData(mapData) {
    if (int(mapData.size()) != rows || (rows > 0 && int(mapData[0].size()) != cols)) {
        // print mapData.size() and mapData[0].size() and rows and cols to debug
        std::cout << "mapData.size(): " << mapData.size() << std::endl;
        std::cout << "mapData[0].size(): " << mapData[0].size() << std::endl;
        std::cout << "rows: " << rows << std::endl;
        std::cout << "cols: " << cols << std::endl;
        // print mapData to debug
        for (int i = 0; i < int(mapData.size()); i++) {
            std::cout << mapData[i] << std::endl;
        }
        throw std::invalid_argument("Map dimensions do not match the provided data.");
    }

    // Initialize bases with default Base objects
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bases[std::make_pair(i, j)] = Base();
        }
    }
}

int Map::getRows() const {
    return rows;
}

int Map::getCols() const {
    return cols;
}

char Map::getCell(int row, int col) const {
    if (!isValidLocation(row, col)) {
        throw std::out_of_range("Cell position out of range.");
    }
    return mapData[row][col];
}

bool Map::isValidLocation(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

bool Map::isRedBase(int row, int col) const {
    return isValidLocation(row, col) && mapData[row][col] == '#';
}

bool Map::isBlueBase(int row, int col) const {
    return isValidLocation(row, col) && mapData[row][col] == '*';
}

bool Map::isNeutralZone(int row, int col) const {
    return isValidLocation(row, col) && mapData[row][col] == '.';
}

std::vector<std::pair<int, int>> Map::getRedBases() const {
    std::vector<std::pair<int, int>> redBases;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (mapData[i][j] == '#') {
                redBases.push_back(std::make_pair(i, j));
            }
        }
    }
    return redBases;
}

std::vector<std::pair<int, int>> Map::getBlueBases() const {
    std::vector<std::pair<int, int>> blueBases;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (mapData[i][j] == '*') {
                blueBases.push_back(std::make_pair(i, j));
            }
        }
    }
    return blueBases;
}

Base* Map::getBaseAtLocation(int row, int col) const {
    auto it = bases.find(std::make_pair(row, col));
    if (it != bases.end()) {
        return const_cast<Base*>(&it->second);
    }
    return nullptr;
}

void Map::setBaseAtLocation(int row, int col, Base& base) {
    bases[std::make_pair(row, col)] = base;
}
