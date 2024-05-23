#include "fighter.h"
#include "map.h"
#include "base.h"

Fighter::Fighter(int id, std::pair<int, int> initialLocation, int fuelCapacity, int maxMissileCarry)
    : id(id), location(initialLocation), fuelCapacity(fuelCapacity), maxMissileCarry(maxMissileCarry), currentFuel(0), currentMissiles(0) {}

int Fighter::getId() const {
    return id;
}

std::pair<int, int> Fighter::getLocation() const {
    return location;
}

int Fighter::getFuelCapacity() const {
    return fuelCapacity;
}

int Fighter::getMaxMissileCarry() const {
    return maxMissileCarry;
}

int Fighter::getCurrentFuel() const {
    return currentFuel;
}

int Fighter::getCurrentMissiles() const {
    return currentMissiles;
}

void Fighter::setLocation(std::pair<int, int> location) {
    this->location = location;
}

void Fighter::setCurrentFuel(int fuel) {
    this->currentFuel = fuel;
}

void Fighter::setCurrentMissiles(int missiles) {
    this->currentMissiles = missiles;
}

bool Fighter::isValidMove(int row, int col) const {
    // Assuming Map is a singleton or globally accessible object
    Map& map = Map::getInstance();
    return map.isValidLocation(row, col);
}

bool Fighter::isAtBlueBase() const {
    // Assuming Map is a singleton or globally accessible object
    Map& map = Map::getInstance();
    return map.isBlueBase(location.first, location.second);
}

bool Fighter::moveUp() {
    if (currentFuel > 0 && isValidMove(location.first - 1, location.second)) {
        location.first -= 1;
        currentFuel -= 1;
        return true;
    }
    return false;
}

bool Fighter::moveDown() {
    if (currentFuel > 0 && isValidMove(location.first + 1, location.second)) {
        location.first += 1;
        currentFuel -= 1;
        return true;
    }
    return false;
}

bool Fighter::moveLeft() {
    if (currentFuel > 0 && isValidMove(location.first, location.second - 1)) {
        location.second -= 1;
        currentFuel -= 1;
        return true;
    }
    return false;
}

bool Fighter::moveRight() {
    if (currentFuel > 0 && isValidMove(location.first, location.second + 1)) {
        location.second += 1;
        currentFuel -= 1;
        return true;
    }
    return false;
}

bool Fighter::attack(int direction, int missileCount) {
    if (currentMissiles < missileCount || missileCount <= 0) {
        return false;
    }

    std::pair<int, int> target;
    switch (direction) {
        case 0: // Up
            target = std::make_pair(location.first - 1, location.second);
            break;
        case 1: // Down
            target = std::make_pair(location.first + 1, location.second);
            break;
        case 2: // Left
            target = std::make_pair(location.first, location.second - 1);
            break;
        case 3: // Right
            target = std::make_pair(location.first, location.second + 1);
            break;
        default:
            return false;
    }

    // Assuming Map is a singleton or globally accessible object
    Map& map = Map::getInstance();
    if (map.isValidLocation(target.first, target.second) && map.isRedBase(target.first, target.second)) {
        currentMissiles -= missileCount;
        // Handle attack logic on the red base
        // e.g., reduce hit points of the red base based on missile count
        Base* base = map.getBaseAtLocation(target.first, target.second);
        if (base) {
            int damage = missileCount;
            base->takeDamage(damage);
        }
        return true;
    }

    return false;
}

bool Fighter::replenishFuel(int amount) {
    if (isAtBlueBase()) {
        Map& map = Map::getInstance();
        Base* base = map.getBaseAtLocation(location.first, location.second);
        if (base && base->getFuelReserve() >= amount) {
            int replenishAmount = std::min(amount, fuelCapacity - currentFuel);
            currentFuel += replenishAmount;
            base->setFuelReserve(base->getFuelReserve() - replenishAmount);
            return true;
        }
    }
    return false;
}

bool Fighter::replenishMissiles(int amount) {
    if (isAtBlueBase()) {
        Map& map = Map::getInstance();
        Base* base = map.getBaseAtLocation(location.first, location.second);
        if (base && base->getMissileReserve() >= amount) {
            int replenishAmount = std::min(amount, maxMissileCarry - currentMissiles);
            currentMissiles += replenishAmount;
            base->setMissileReserve(base->getMissileReserve() - replenishAmount);
            return true;
        }
    }
    return false;
}
