// Base.cpp
#include "Base.h"

Base::Base(int x, int y, int fuelReserve, int missileReserve, int hitPoints, int reward, char type)
    : x(x), y(y), fuelReserve(fuelReserve), missileReserve(missileReserve), hitPoints(hitPoints), reward(reward), type(type) {}

int Base::getX() const {
    return x;
}

int Base::getY() const {
    return y;
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

int Base::getReward() const {
    return reward;
}

char Base::getType() const {
    return type;
}

void Base::setFuelReserve(int fuel) {
    fuelReserve = fuel;
}

void Base::setMissileReserve(int missiles) {
    missileReserve = missiles;
}

void Base::setHitPoints(int hp) {
    hitPoints = hp;
}

void Base::replenishFuel(int& fighterFuel) {
    if (fuelReserve > 0) {
        fighterFuel += fuelReserve;
        fuelReserve = 0;
    }
}

void Base::replenishMissiles(int& fighterMissiles) {
    if (missileReserve > 0) {
        fighterMissiles += missileReserve;
        missileReserve = 0;
    }
}

void Base::takeDamage(int damage) {
    hitPoints -= damage;
}

bool Base::isDestroyed() const {
    return hitPoints <= 0;
}