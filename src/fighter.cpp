// Fighter.cpp
#include "Fighter.h"
#include <cmath>

Fighter::Fighter(int id, int row, int col, int fuelCapacity, int missileCapacity, int mapRows, int mapCols)
    : id(id), row(row), col(col), fuelCapacity(fuelCapacity), missileCapacity(missileCapacity),
      currentFuel(0), currentMissiles(0), mapRows(mapRows), mapCols(mapCols) {}

int Fighter::getId() const {
    return id;
}

int Fighter::getRow() const {
    return row;
}

int Fighter::getCol() const {
    return col;
}

int Fighter::getFuelCapacity() const {
    return fuelCapacity;
}

int Fighter::getMissileCapacity() const {
    return missileCapacity;
}

int Fighter::getCurrentFuel() const {
    return currentFuel;
}

int Fighter::getCurrentMissiles() const {
    return currentMissiles;
}

bool Fighter::move(int direction) {
    int newRow = row;
    int newCol = col;

    switch (direction) {
        case 0:  // Up
            newRow--;
            break;
        case 1:  // Down
            newRow++;
            break;
        case 2:  // Left
            newCol--;
            break;
        case 3:  // Right
            newCol++;
            break;
    }

    if (isWithinBounds(newRow, newCol) && hasFuel()) {
        row = newRow;
        col = newCol;
        consumeFuel();
        return true;
    }
    return false;
}

void Fighter::refuel() {
    currentFuel = fuelCapacity;
}

void Fighter::reloadMissiles() {
    currentMissiles = missileCapacity;
}

bool Fighter::launchMissile(int targetRow, int targetCol) {
    if (currentMissiles > 0 && (std::abs(targetRow - row) == 1 && targetCol == col) || (targetRow == row && std::abs(targetCol - col) == 1)) {
        currentMissiles--;
        return true;
    }
    return false;
}

bool Fighter::hasFuel() const {
    return currentFuel > 0;
}

bool Fighter::hasMissiles() const {
    return currentMissiles > 0;
}

void Fighter::consumeFuel() {
    if (currentFuel > 0) {
        currentFuel--;
    }
}

bool Fighter::isWithinBounds(int row, int col) const {
    return row >= 0 && row < mapRows && col >= 0 && col < mapCols;
}