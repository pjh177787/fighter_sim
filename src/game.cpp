// main.cpp
#include <iostream>
#include "Map.h"
#include "Fighter.h"

int main() {
    Map map(6, 9);
    map.setBlueBase(0, 6);
    map.setBlueBase(3, 7);
    map.setBlueBase(4, 6);
    map.setBlueBase(4, 7);
    map.setBlueBase(4, 8);
    map.setRedBase(1, 2);
    map.setRedBase(2, 1);
    map.setRedBase(2, 3);
    map.setRedBase(5, 3);

    Fighter fighter(1, 0, 6, 10, 5, map.getRows(), map.getCols());

    std::cout << "Fighter ID: " << fighter.getId() << std::endl;
    std::cout << "Fighter Position: (" << fighter.getRow() << ", " << fighter.getCol() << ")" << std::endl;
    std::cout << "Fighter Fuel Capacity: " << fighter.getFuelCapacity() << std::endl;
    std::cout << "Fighter Missile Capacity: " << fighter.getMissileCapacity() << std::endl;

    fighter.refuel();
    fighter.reloadMissiles();

    std::cout << "Fighter Current Fuel: " << fighter.getCurrentFuel() << std::endl;
    std::cout << "Fighter Current Missiles: " << fighter.getCurrentMissiles() << std::endl;

    fighter.move(1);  // Move down
    std::cout << "Fighter New Position: (" << fighter.getRow() << ", " << fighter.getCol() << ")" << std::endl;

    fighter.launchMissile(1, 2);  // Launch missile at red base
    std::cout << "Fighter Current Missiles: " << fighter.getCurrentMissiles() << std::endl;

    return 0;
}