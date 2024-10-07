//
// Created by LUMI on 2024-10-02.
//

#ifndef MINESWEEPER_GAME_OBJECTS_H
#define MINESWEEPER_GAME_OBJECTS_H

#include <vector>
#include <cstdint>

struct Cell {
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int32_t adjacentMines;
};

enum GameStatus {
    ERROR = -1,
    STARTED = 0,
    ONGOING = 1,
    STEPPED_MINE = 2,
    VICTORY = 3
};

class GameBoard {
public:
    GameBoard(int32_t width, int32_t height, int32_t mineCount);

    void initializeBoard(int32_t firstClickX, int32_t firstClickY);

    void revealCell(int32_t x, int32_t y);

    void toggleFlag(int32_t x, int32_t y);

    void updateGameStatus();

    const Cell &getCell(int32_t x, int32_t y) const;

    int32_t getWidth() const;

    int32_t getHeight() const;

    GameStatus state;

private:
    void placeMines(int32_t firstClickX, int32_t firstClickY);

    void calculateAdjacentMines();

    bool isInBounds(int32_t x, int32_t y) const;

    int32_t width;
    int32_t height;
    int32_t mineCount;
    std::vector<std::pair<int32_t, int32_t>> mines;
    std::vector<std::vector<Cell>> board;
};

#endif //MINESWEEPER_GAME_OBJECTS_H
