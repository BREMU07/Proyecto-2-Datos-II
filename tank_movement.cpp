#include <iostream>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <algorithm>

using namespace std;

class TankMovement {
public:
    struct Position {
        int row, col;

        bool operator<(const Position& other) const {
            if (row == other.row) {
                return col < other.col;
            }
            return row < other.row;
        }
    };

    TankMovement(int rows, int cols) : ROWS(rows), COLS(cols) {
        rowMoves = {-1, 1, 0, 0};
        colMoves = {0, 0, -1, 1};
    }

    vector<Position> calculateMove(Position start, Position goal, const vector<vector<int>>& map, bool useBFS, bool useDijkstra) {
        if (useBFS) {
            cout << "Usando BFS para calcular la ruta.\n";
            return bfs(start, goal, map);
        } else if (useDijkstra) {
            cout << "Usando Dijkstra para calcular la ruta.\n";
            return dijkstra(start, goal, map);
        } else {
            cout << "Usando movimiento aleatorio.\n";
            return randomPath(start, goal, map);
        }
    }

    void addObstacles(vector<vector<int>>& map) {
        map[3][4] = 1;
        map[4][4] = 1;
        map[5][4] = 1;
        map[2][8] = 1;
    }

    void printMapWithPath(const vector<vector<int>>& map, const vector<Position>& path, Position start, Position goal) {
        vector<vector<char>> visualMap(ROWS, vector<char>(COLS, '.'));

        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (map[i][j] == 1) {
                    visualMap[i][j] = '#';
                }
            }
        }

        visualMap[start.row][start.col] = 'S';
        visualMap[goal.row][goal.col] = 'G';

        for (const Position& pos : path) {
            if (visualMap[pos.row][pos.col] != 'S' && visualMap[pos.row][pos.col] != 'G') {
                visualMap[pos.row][pos.col] = '*';
            }
        }

        for (const auto& row : visualMap) {
            for (char cell : row) {
                cout << cell << ' ';
            }
            cout << '\n';
        }
    }

private:
    const int ROWS;
    const int COLS;
    vector<int> rowMoves;
    vector<int> colMoves;

    bool isValidMove(int row, int col, const vector<vector<int>>& map) {
        return (row >= 0 && row < ROWS && col >= 0 && col < COLS && map[row][col] == 0);
    }

    vector<Position> bfs(Position start, Position goal, const vector<vector<int>>& map) {
        vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));
        vector<vector<Position>> previous(ROWS, vector<Position>(COLS, {-1, -1}));
        queue<Position> q;

        q.push(start);
        visited[start.row][start.col] = true;

        while (!q.empty()) {
            Position current = q.front();
            q.pop();

            if (current.row == goal.row && current.col == goal.col) {
                vector<Position> path;
                for (Position at = goal; at.row != -1; at = previous[at.row][at.col]) {
                    path.push_back(at);
                }
                reverse(path.begin(), path.end());
                return path;
            }

            for (int i = 0; i < 4; i++) {
                int newRow = current.row + rowMoves[i];
                int newCol = current.col + colMoves[i];

                if (isValidMove(newRow, newCol, map) && !visited[newRow][newCol]) {
                    q.push({newRow, newCol});
                    visited[newRow][newCol] = true;
                    previous[newRow][newCol] = current;
                }
            }
        }

        return {};
    }

    vector<Position> dijkstra(Position start, Position goal, const vector<vector<int>>& map) {
        vector<vector<int>> dist(ROWS, vector<int>(COLS, INT_MAX));
        vector<vector<Position>> previous(ROWS, vector<Position>(COLS, {-1, -1}));
        priority_queue<pair<int, Position>, vector<pair<int, Position>>, greater<>> pq;

        pq.push({0, start});
        dist[start.row][start.col] = 0;

        while (!pq.empty()) {
            Position current = pq.top().second;
            int currentDist = pq.top().first;
            pq.pop();

            if (current.row == goal.row && current.col == goal.col) {
                vector<Position> path;
                for (Position at = goal; at.row != -1; at = previous[at.row][at.col]) {
                    path.push_back(at);
                }
                reverse(path.begin(), path.end());
                return path;
            }

            for (int i = 0; i < 4; i++) {
                int newRow = current.row + rowMoves[i];
                int newCol = current.col + colMoves[i];

                if (isValidMove(newRow, newCol, map)) {
                    int newDist = currentDist + 1;
                    if (newDist < dist[newRow][newCol]) {
                        dist[newRow][newCol] = newDist;
                        pq.push({newDist, {newRow, newCol}});
                        previous[newRow][newCol] = current;
                    }
                }
            }
        }

        return {};
    }

    Position randomMove(Position current, const vector<vector<int>>& map) {
        vector<Position> validMoves;

        for (int i = 0; i < 4; i++) {
            int newRow = current.row + rowMoves[i];
            int newCol = current.col + colMoves[i];
            if (isValidMove(newRow, newCol, map)) {
                validMoves.push_back({newRow, newCol});
            }
        }

        if (!validMoves.empty()) {
            int randomIndex = rand() % validMoves.size();
            return validMoves[randomIndex];
        }

        return current;
    }

    vector<Position> randomPath(Position start, Position goal, const vector<vector<int>>& map) {
        vector<Position> path;
        Position current = start;
        path.push_back(current);

        while (current.row != goal.row || current.col != goal.col) {
            Position nextMove = randomMove(current, map);

            if (nextMove.row == current.row && nextMove.col == current.col) {
                break;
            }

            path.push_back(nextMove);
            current = nextMove;
        }

        return path;
    }
};


//int main() {
//    srand(time(0));

//    const int ROWS = 10;
//    const int COLS = 10;
//    TankMovement tankMovement(ROWS, COLS);

//    vector<vector<int>> map(ROWS, vector<int>(COLS, 0));
//    tankMovement.addObstacles(map);

//    TankMovement::Position start = {0, 0};
//    TankMovement::Position goal = {9, 9};

//    bool useBFS = rand() % 100 < 50;       // 50% de probabilidad de usar BFS
//    bool useDijkstra = rand() % 100 < 80;  // 80% de probabilidad de usar Dijkstra

//    vector<TankMovement::Position> path_lightblue_blue = tankMovement.calculateMove(start, goal, map, useBFS, 0);
//    vector<TankMovement::Position> path_yellow_red = tankMovement.calculateMove(start, goal, map, 0, useDijkstra);
//    tankMovement.printMapWithPath(map, path_lightblue_blue, start, goal);
//    tankMovement.printMapWithPath(map, path_yellow_red, start, goal);

//    return 0;
//}

