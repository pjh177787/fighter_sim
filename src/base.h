// Base.h
#ifndef BASE_H
#define BASE_H

class Base {
private:
    int x;
    int y;
    int fuelReserve;
    int missileReserve;
    int hitPoints;
    int reward;
    char type;

public:
    Base(int x, int y, int fuelReserve, int missileReserve, int hitPoints, int reward, char type);

    int getX() const;
    int getY() const;
    int getFuelReserve() const;
    int getMissileReserve() const;
    int getHitPoints() const;
    int getReward() const;
    char getType() const;

    void setFuelReserve(int fuel);
    void setMissileReserve(int missiles);
    void setHitPoints(int hp);

    void replenishFuel(int& fighterFuel);
    void replenishMissiles(int& fighterMissiles);
    void takeDamage(int damage);
    bool isDestroyed() const;
};

#endif