#ifndef BASE_H
#define BASE_H

#include <utility> // for std::pair
#include <string>

class Base {
public:
    // Default constructor
    Base();

    // Parameterized constructor to initialize the base with attributes
    Base(std::pair<int, int> location, int fuelReserve, int missileReserve, int hitPoints, int militaryValue, const std::string& team);

    // Getters for base attributes
    std::pair<int, int> getLocation() const;
    int getFuelReserve() const;
    int getMissileReserve() const;
    int getHitPoints() const;
    int getMilitaryValue() const;
    std::string getTeam() const;
    bool isActive() const;

    // Setters for base attributes
    void setFuelReserve(int fuelReserve);
    void setMissileReserve(int missileReserve);
    void setHitPoints(int hitPoints);
    void setMilitaryValue(int militaryValue);
    void setTeam(const std::string& team);

    // Method to take damage
    void takeDamage(int damage);

private:
    std::pair<int, int> location;
    int fuelReserve;
    int missileReserve;
    int hitPoints;
    int militaryValue;
    std::string team;
    bool active;
};

#endif // BASE_H
