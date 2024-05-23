#ifndef FIGHTER_H
#define FIGHTER_H

#include <utility> // for std::pair

class Fighter {
public:
    // Constructor to initialize the fighter with attributes
    Fighter(int id, std::pair<int, int> initialLocation, int fuelCapacity, int maxMissileCarry);

    // Getters for fighter attributes
    int getId() const;
    std::pair<int, int> getLocation() const;
    int getFuelCapacity() const;
    int getMaxMissileCarry() const;
    int getCurrentFuel() const;
    int getCurrentMissiles() const;

    // Setters for fighter attributes
    void setLocation(std::pair<int, int> location);
    void setCurrentFuel(int fuel);
    void setCurrentMissiles(int missiles);

    // Methods to move the fighter
    bool moveUp();
    bool moveDown();
    bool moveLeft();
    bool moveRight();

    // Method to attack (returns true if attack is successful)
    bool attack(int direction, int missileCount);

    // Methods to replenish fuel and missiles
    bool replenishFuel(int amount);
    bool replenishMissiles(int amount);

private:
    int id;
    std::pair<int, int> location;
    int fuelCapacity;
    int maxMissileCarry;
    int currentFuel;
    int currentMissiles;

    bool isValidMove(int row, int col) const; // Helper method to check if a move is valid
    bool isAtBlueBase() const; // Helper method to check if the fighter is at a blue base
};

#endif // FIGHTER_H
