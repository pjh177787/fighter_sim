// Map.h
#ifndef MAP_H
#define MAP_H

#include <vector>
#include <utility>

class Map {
private:
    int rows;
    int cols;
    std::vector<std::vector<char>> grid;

public:
    Map(int rows, int cols);

    void setRedBase(int row, int col);
    void setBlueBase(int row, int col);

    bool isRedBase(int row, int col) const;
    bool isBlueBase(int row, int col) const;
    bool isNeutralArea(int row, int col) const;

    void display() const;

    int getRows() const;
    int getCols() const;

    std::vector<std::pair<int, int>> getRedBaseLocations() const;
    std::vector<std::pair<int, int>> getBlueBaseLocations() const;

private:
    bool isWithinBounds(int row, int col) const;
};

#endif