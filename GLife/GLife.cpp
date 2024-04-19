#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <thread>
#include <chrono>

const int gridDimension = 20;
const int gameIterations = 50;

void displayGrid(bool universe[gridDimension][gridDimension]) {
    std::cout << "+" << std::string(gridDimension * 2, '-') << "+" << std::endl;

    for (int row = 0; row < gridDimension; row++) {
        std::cout << "|";
        for (int col = 0; col < gridDimension; col++) {
            std::cout << (universe[row][col] ? '*' : ' ') << ' ';
        }
        std::cout << "|" << std::endl;
    }

    std::cout << "+" << std::string(gridDimension * 2, '-') << "+" << std::endl << std::endl;
}

int neighborCount(bool universe[gridDimension][gridDimension], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0) && universe[(x + i + gridDimension) % gridDimension][(y + j + gridDimension) % gridDimension]) {
                count++;
            }
        }
    }
    return count;
}

void evolveUniverse(bool universe[gridDimension][gridDimension]) {
    bool newUniverse[gridDimension][gridDimension];

#pragma omp parallel for
    for (int row = 0; row < gridDimension; row++) {
        for (int col = 0; col < gridDimension; col++) {
            int neighbors = neighborCount(universe, row, col);
            newUniverse[row][col] = universe[row][col] ? (neighbors == 2 || neighbors == 3) : (neighbors == 3);
        }
    }

#pragma omp parallel for
    for (int row = 0; row < gridDimension; row++) {
        for (int col = 0; col < gridDimension; col++) {
            universe[row][col] = newUniverse[row][col];
        }
    }
}

void initializeUniverse(bool universe[gridDimension][gridDimension]) {
    for (int row = 0; row < gridDimension; row++) {
        for (int col = 0; col < gridDimension; col++) {
            universe[row][col] = rand() % 2;
        }
    }
}

int main() {
    bool universe[gridDimension][gridDimension];
    initializeUniverse(universe);

    for (int iteration = 0; iteration < gameIterations; iteration++) {
        std::cout << "Iteration: " << iteration << std::endl;
        displayGrid(universe);
        evolveUniverse(universe);
    }

    return 0;
}
