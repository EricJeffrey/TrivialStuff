
#include "Board.h"
#include "TerminalCtl.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <random>
#include <string>
#include <vector>

termios TerminalCtl::ttyOrigin;

using std::string;
using std::vector;

/*
    ↑ 27 91 65
    ↓ 27 91 66
    ← 27 91 68
    → 27 91 67
    ctrl c 3
    esc 27
"\
 ---------------------------  \r\n\
| 1024 | 1024 | 1024 | 1024 |\r\n\
| 1024 | 1024 | 1024 | 1024 |\r\n\
| 1024 | 1024 | 1024 | 1024 |\r\n\
| 1024 | 1024 | 1024 | 1024 |\r\n\
 ---------------------------  \r\n
*/

const long Board::EMPTY_CELL = -1;
const size_t Board::ROW_COL_COUNT = 3;

// return a random between [a, b]
int randInt(int a, int b) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(a, b); // included [a, b]
    return distribution(generator);
}
void redrawBoard(const Board &board, bool clearScreen = true) {
    if (clearScreen)
        TerminalCtl::clearScreen();
    const size_t dashCount = 6 * Board::ROW_COL_COUNT - 1;
    fprintf(stderr, " ");
    for (size_t i = 0; i < dashCount; i++)
        fprintf(stderr, "-");
    fprintf(stderr, "\r\n");
    for (size_t i = 0; i < Board::ROW_COL_COUNT; i++) {
        for (size_t j = 0; j < Board::ROW_COL_COUNT; j++) {
            long val = board.data[i][j];
            if (val == -1)
                fprintf(stderr, "|     ");
            else
                fprintf(stderr, "| %3ld ", val);
        }
        fprintf(stderr, "|\r\n");
        if (i != Board::ROW_COL_COUNT - 1) {
            fprintf(stderr, "|");
            for (size_t j = 0; j < dashCount; j++)
                fprintf(stderr, "-");
            fprintf(stderr, "|\r\n");
        }
    }
    fprintf(stderr, " ");
    for (size_t i = 0; i < dashCount; i++)
        fprintf(stderr, "-");
    fprintf(stderr, "\r\n");
}

void work() {
    atexit(TerminalCtl::ttyReset);
    TerminalCtl::ttySetRaw(STDOUT_FILENO);
    Board board;
    bool canShift = true;
    while (true) {
        vector<Pos> availablePos = board.getEmptyPos();
        if (canShift) {
            if (!availablePos.empty()) {
                int who = randInt(0, availablePos.size() - 1);
                long val = randInt(1, 2) * 2;
                board.setCell(availablePos[who].i, availablePos[who].j, val);
            }
            redrawBoard(board);
        }
        if (!board.canMove()) {
            usleep(1500000);
            fprintf(stderr, "Game Over...\r\n");
            break;
        }
        const int bufsize = 1024;
        char buf[bufsize] = {};
        ssize_t numRead = read(STDIN_FILENO, buf, bufsize);
        if (numRead == -1) {
            fprintf(stderr, "ERROR! Read from stdin failed: %s\r\n", strerror(errno));
            break;
        } else if (numRead == 0) {
            fprintf(stderr, "ERROR! Read from stdin returned with 0");
            break;
        } else {
            if (numRead >= 1 && buf[0] == 3) {
                fprintf(stderr, "Exiting\r\n");
                break;
            } else if (numRead >= 3) {
                Board::Direction directions[4] = {
                    Board::Direction::top,
                    Board::Direction::bottom,
                    Board::Direction::left,
                    Board::Direction::right,
                };
                vector<vector<int>> dirSequence = {
                    {27, 91, 65},
                    {27, 91, 66},
                    {27, 91, 68},
                    {27, 91, 67},
                };
                Board::Direction direction = Board::Direction::invalid;
                bool gotDirection = false;
                for (size_t i = 0; !gotDirection && i < dirSequence.size(); i++) {
                    for (size_t j = 0; j < 3; j++) {
                        if (dirSequence[i][j] != buf[j])
                            break;
                        else if (j == 2) {
                            direction = directions[i];
                            gotDirection = true;
                        }
                    }
                }
                if (gotDirection)
                    canShift = board.shift(direction);
            } else
                ;
        }
    }
}

/*

g++ -g -Wall -std=c++17 -o main.out main.cpp

*/

int main(int argc, char const *argv[]) {
    work();
    return 0;
    {
        Board board;
        board.data = {{4, 8, 2, 32}, {-1, -1, -1, 2}, {-1, -1, -1, 2}, {-1, -1, -1, -1}};
        redrawBoard(board);
        board.shift(Board::Direction::top);
        redrawBoard(board, false);
    }
    {
        atexit(TerminalCtl::ttyReset);
        TerminalCtl::ttySetRaw(STDOUT_FILENO);
        while (true) {
            const int bufsize = 1024;
            char buf[bufsize] = {};
            ssize_t numRead = read(STDIN_FILENO, buf, bufsize);
            if (numRead == -1) {
                perror("ERROR");
                break;
            }
            if (buf[0] == 3)
                break;
            for (ssize_t i = 0; i < numRead; i++)
                fprintf(stderr, "%d ", buf[i]);
            fprintf(stderr, "\r\n");
        }
    }

    return 0;
}
