#include <vector>
#include "game_objects.h"
#include <random>
#include <algorithm>

GameBoard::GameBoard(int32_t width, int32_t height, int32_t mineCount) {
    this->width = width;
    this->height = height;
    this->mineCount = mineCount;
    this->board.resize(height, std::vector<Cell>(width));
    this->state = STARTED;
}

void GameBoard::initializeBoard(int32_t firstClickX, int32_t firstClickY) {
    placeMines(firstClickX, firstClickY);
    calculateAdjacentMines();
    this->state = ONGOING;
}

void GameBoard::revealCell(int32_t x, int32_t y) {
    if (board[y][x].isMine) {
        this->state = STEPPED_MINE;
    }
    board[y][x].isRevealed = true;

    std::vector<std::pair<int32_t, int32_t>> stack;
    stack.emplace_back(x, y);
    const std::vector<std::pair<int32_t, int32_t>> DELTA2 = {{0,  1},
                                                             {0,  -1},
                                                             {1,  0},
                                                             {-1, 0}};

    while (not stack.empty()) {
        const auto [currentX, currentY] = stack.back();
        stack.pop_back();
        for (const auto [dx, dy]: DELTA2) {
            int32_t nextX = currentX + dx;
            int32_t nextY = currentY + dy;
            if (not isInBounds(nextX, nextY)) {
                continue;
            }
            if (board[nextY][nextX].isRevealed) {
                continue;
            }
            if (board[nextY][nextX].isFlagged) {
                continue;
            }
            if (board[nextY][nextX].isMine) {
                continue;
            }
            board[nextY][nextX].isMine = true;
            stack.emplace_back(nextX, nextY);
        }
    }
}

void GameBoard::toggleFlag(int32_t x, int32_t y) {
    if (isInBounds(x, y) and not board[y][x].isRevealed) {
        board[y][x].isFlagged = not board[y][x].isFlagged;
    }
}

void GameBoard::updateGameStatus() {
    if (state == STEPPED_MINE or state == VICTORY) {
        return;
    }
    for (const auto &row: board) {
        for (const auto &cell: row) {
            if (not cell.isMine and not cell.isRevealed) {
                return;
            }
        }
    }
    state = VICTORY
}

const Cell &GameBoard::getCell(int32_t x, int32_t y) const {
    return this->board[y][x];
}

int GameBoard::getWidth() const {
    return this->width;
}

int GameBoard::getHeight() const {
    return this->height;
}

void GameBoard::placeMines(int32_t firstClickX, int32_t firstClickY) {
    std::vector<std::pair<int32_t, int32_t>> positions;
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            if (not(x == firstClickX and y == firstClickY)) {
                positions.emplace_back(x, y);
            }
        }
    }

    std::shuffle(positions.begin(), positions.end(), std::mt19937());
    for (int32_t i = 0; i < mineCount; i++) {
        int32_t x = positions[i].first;
        int32_t y = positions[i].second;
        board[y][x].isMine = true;
    }
}

void GameBoard::calculateAdjacentMines() {
    const std::vector<std::pair<int32_t, int32_t>> DELTA = {{0,  1},
                                                            {0,  -1},
                                                            {1,  0},
                                                            {-1, 0},
                                                            {-1, -1},
                                                            {-1, 1},
                                                            {1,  -1},
                                                            {1,  1}};

    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            if (not board[y][x].isMine) {
                int32_t count = 0;
                for (const auto &delta: DELTA) {
                    int32_t nx = x + delta.first;
                    int32_t ny = y + delta.second;
                    if (isInBounds(nx, ny) and board[ny][nx].isMine) {
                        count += 1;
                    }
                }
                board[y][x].adjacentMines = count;
            }
        }
    }
}

bool GameBoard::isInBounds(int32_t x, int32_t y) const {
    return 0 <= x and x < this->width and 0 <= y and y < this->height;
}