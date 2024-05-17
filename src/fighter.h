// Fighter.h
#ifndef FIGHTER_H
#define FIGHTER_H

class Fighter {
private:
    int id;
    int row;
    int col;
    int fuelCapacity;
    int missileCapacity;
    int currentFuel;
    int currentMissiles;
    int mapRows;
    int mapCols;

public:
    Fighter(int id, int row, int col, int fuelCapacity, int missileCapacity, int mapRows, int mapCols);

    int getId() const;
    int getRow() const;
    int getCol() const;
    int getFuelCapacity() const;
    int getMissileCapacity() const;
    int getCurrentFuel() const;
    int getCurrentMissiles() const;

    bool move(int direction);
    void refuel();
    void reloadMissiles();
    bool launchMissile(int targetRow, int targetCol);
    bool hasFuel() const;
    bool hasMissiles() const;

private:
    void consumeFuel();
    bool isWithinBounds(int row, int col) const;
};

#endif