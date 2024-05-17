#include <iostream>
#include <vector>
#include <utility>

class Map {
private:
    int rows;
    int cols;
    std::vector<std::vector<char>> grid;

public:
    Map(int rows, int cols) : rows(rows), cols(cols) {
        grid.resize(rows, std::vector<char>(cols, '.'));
    }

    void setRedBase(int row, int col) {
        if (isWithinBounds(row, col)) {
            grid[row][col] = '#';
        }
    }

    void setBlueBase(int row, int col) {
        if (isWithinBounds(row, col)) {
            grid[row][col] = '*';
        }
    }

    bool isRedBase(int row, int col) const {
        if (isWithinBounds(row, col)) {
            return grid[row][col] == '#';
        }
        return false;
    }

    bool isBlueBase(int row, int col) const {
        if (isWithinBounds(row, col)) {
            return grid[row][col] == '*';
        }
        return false;
    }

    bool isNeutralArea(int row, int col) const {
        if (isWithinBounds(row, col)) {
            return grid[row][col] == '.';
        }
        return false;
    }

    void display() const {
        for (const auto& row : grid) {
            for (char cell : row) {
                std::cout << cell << " ";
            }
            std::cout << std::endl;
        }
    }

    int getRows() const {
        return rows;
    }

    int getCols() const {
        return cols;
    }

    std::vector<std::pair<int, int>> getRedBaseLocations() const {
        std::vector<std::pair<int, int>> locations;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (isRedBase(row, col)) {
                    locations.emplace_back(row, col);
                }
            }
        }
        return locations;
    }

    std::vector<std::pair<int, int>> getBlueBaseLocations() const {
        std::vector<std::pair<int, int>> locations;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (isBlueBase(row, col)) {
                    locations.emplace_back(row, col);
                }
            }
        }
        return locations;
    }

private:
    bool isWithinBounds(int row, int col) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }
};