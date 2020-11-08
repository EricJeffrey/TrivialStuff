#if !defined(BOARD_H)
#define BOARD_H

#include <cstdio>

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

struct Pos {
    size_t i, j;
};

struct Board {
    static const size_t ROW_COL_COUNT;
    static const long EMPTY_CELL;

    enum Direction { top, bottom, left, right, invalid };

    vector<vector<long>> data;

    Board() : data(ROW_COL_COUNT, vector<long>(ROW_COL_COUNT, EMPTY_CELL)) {}
    ~Board() {}

    vector<Pos> getEmptyPos() {
        vector<Pos> resPosList;
        for (size_t i = 0; i < ROW_COL_COUNT; i++)
            for (size_t j = 0; j < ROW_COL_COUNT; j++)
                if (data[i][j] == EMPTY_CELL)
                    resPosList.push_back({i, j});
        return resPosList;
    }

    void setCell(size_t i, size_t j, long val) { data[i][j] = val; }

    bool canMove() {
        for (size_t i = 0; i < ROW_COL_COUNT; i++)
            for (size_t j = 0; j < ROW_COL_COUNT; j++)
                if (data[i][j] == EMPTY_CELL)
                    return true;
        for (size_t i = 0; i < ROW_COL_COUNT; i++)
            for (size_t j = 1; j < ROW_COL_COUNT; j++)
                if (data[i][j] == data[i][j - 1])
                    return true;
        for (size_t j = 0; j < ROW_COL_COUNT; j++)
            for (size_t i = 1; i < ROW_COL_COUNT; i++)
                if (data[i][j] == data[i - 1][j])
                    return true;
        return false;
    }

    bool shift(Direction direction) {
        auto convertIJ = [&direction, this](size_t rawi, size_t rawj) -> std::pair<size_t, size_t> {
            using std::make_pair;
            switch (direction) {
                case Direction::top:
                    return make_pair(rawj, rawi);
                case Direction::bottom:
                    return make_pair(ROW_COL_COUNT - rawj - 1, rawi);
                case Direction::left:
                    return make_pair(rawi, rawj);
                case Direction::right:
                    return make_pair(rawi, ROW_COL_COUNT - rawj - 1);
                case Direction::invalid:
                    return make_pair(-1, -1);
            }
            return make_pair(0, 0);
        };
        auto valueOf = [&direction, &convertIJ, this](size_t rawi, size_t rawj) -> long & {
            auto [ni, nj] = convertIJ(rawi, rawj);
            return data[ni][nj];
        };
        auto oldData = data;
        for (size_t i = 0; i < ROW_COL_COUNT; i++) {
            vector<long> oldData = data[i];
            vector<long> values;
            for (size_t j = 0; j < ROW_COL_COUNT; j++)
                if (valueOf(i, j) != EMPTY_CELL)
                    values.push_back(valueOf(i, j));
            // sum them and put back into board
            if (!values.empty()) {
                size_t ni = 0, nj = 1;
                while (nj < values.size()) {
                    if (values[nj] == values[ni]) {
                        values[ni] = EMPTY_CELL;
                        values[nj] *= 2;
                        ni = nj + 1;
                        nj = nj + 2;
                    } else {
                        ni = nj;
                        nj += 1;
                    }
                }
                for (size_t j = 0; j < ROW_COL_COUNT; j++)
                    valueOf(i, j) = EMPTY_CELL;
                size_t j = 0;
                for (auto &&v : values)
                    if (v != EMPTY_CELL) {
                        valueOf(i, j) = v;
                        j++;
                    }
            }
        }
        for (size_t i = 0; i < ROW_COL_COUNT; i++)
            for (size_t j = 0; j < ROW_COL_COUNT; j++)
                if (oldData[i][j] != data[i][j])
                    return true;
        return false;
    }
};

#endif // BOARD_H
