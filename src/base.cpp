#include "base.h"

// Default constructor
Base::Base()
    : location({0, 0}), fuelReserve(0), missileReserve(0), hitPoints(0), militaryValue(0), team("neutral"), active(true) {}

Base::Base(std::pair<int, int> location, int fuelReserve, int missileReserve, int hitPoints, int militaryValue, const std::string& team)
    : location(location), fuelReserve(fuelReserve), missileReserve(missileReserve), hitPoints(hitPoints), militaryValue(militaryValue), team(team), active(true) {}

std::pair<int, int> Base::getLocation() const {
    return location;
}

int Base::getFuelReserve() const {
    return fuelReserve;
}

int Base::getMissileReserve() const {
    return missileReserve;
}

int Base::getHitPoints() const {
    return hitPoints;
}

int Base::getMilitaryValue() const {
    return militaryValue;
}

std::string Base::getTeam() const {
    return team;
}

bool Base::isActive() const {
    return active;
}

void Base::setFuelReserve(int fuelReserve) {
    this->fuelReserve = fuelReserve;
}

void Base::setMissileReserve(int missileReserve) {
    this->missileReserve = missileReserve;
}

void Base::setHitPoints(int hitPoints) {
    this->hitPoints = hitPoints;
    if (this->hitPoints <= 0) {
        this->hitPoints = 0;
        active = false;
    }
}

void Base::setMilitaryValue(int militaryValue) {
    this->militaryValue = militaryValue;
}

void Base::setTeam(const std::string& team) {
    this->team = team;
}

void Base::takeDamage(int damage) {
    setHitPoints(this->hitPoints - damage);
}
