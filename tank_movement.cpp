#include <iostream>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <algorithm>  // Necesario para std::reverse

using namespace std;

const int ROWS = 10;  // Tamaño del mapa
const int COLS = 10;  // Tamaño del mapa

// Direcciones de movimiento (arriba, abajo, izquierda, derecha)
int rowMoves[] = {-1, 1, 0, 0};
int colMoves[] = {0, 0, -1, 1};

// Estructura para guardar las coordenadas del tanque
struct Position {
    int row, col;

    // Sobrecargar el operador < para permitir comparaciones
    bool operator<(const Position& other) const {
        if (row == other.row) {
            return col < other.col;  // Comparar por columna si las filas son iguales
        }
        return row < other.row;  // Comparar por fila
    }
};

// Función para verificar si la nueva posición es válida
bool isValidMove(int row, int col, const vector<vector<int>>& map) {
    return (row >= 0 && row < ROWS && col >= 0 && col < COLS && map[row][col] == 0);  // Verifica que no haya obstáculo
}

// Implementación de BFS para encontrar el camino más corto
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
            // Se ha encontrado la meta, reconstruir la ruta
            vector<Position> path;
            for (Position at = goal; at.row != -1; at = previous[at.row][at.col]) {
                path.push_back(at);
            }
            reverse(path.begin(), path.end());  // Revertir la ruta para obtener el camino correcto
            return path;
        }

        // Intentar moverse en todas las direcciones posibles
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

    return {};  // No se encontró una ruta
}

// Implementación de Dijkstra para encontrar el camino más corto en un grafo ponderado
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
            // Se ha encontrado la meta, reconstruir la ruta
            vector<Position> path;
            for (Position at = goal; at.row != -1; at = previous[at.row][at.col]) {
                path.push_back(at);
            }
            reverse(path.begin(), path.end());  // Revertir la ruta
            return path;
        }

        for (int i = 0; i < 4; i++) {
            int newRow = current.row + rowMoves[i];
            int newCol = current.col + colMoves[i];

            if (isValidMove(newRow, newCol, map)) {
                int newDist = currentDist + 1;  // Asignar el peso (en este caso, 1 para cada movimiento)
                if (newDist < dist[newRow][newCol]) {
                    dist[newRow][newCol] = newDist;
                    pq.push({newDist, {newRow, newCol}});
                    previous[newRow][newCol] = current;
                }
            }
        }
    }

    return {};  // No se encontró una ruta
}

// Implementación de movimiento aleatorio
Position randomMove(Position current, const vector<vector<int>>& map) {
    vector<Position> validMoves;

    // Verificar todas las direcciones válidas
    for (int i = 0; i < 4; i++) {
        int newRow = current.row + rowMoves[i];
        int newCol = current.col + colMoves[i];
        if (isValidMove(newRow, newCol, map)) {
            validMoves.push_back({newRow, newCol});
        }
    }

    if (!validMoves.empty()) {
        // Seleccionar un movimiento aleatorio
        int randomIndex = rand() % validMoves.size();
        return validMoves[randomIndex];
    }

    return current;  // Si no hay movimientos válidos, quedarse en el mismo lugar
}

// Implementación de movimiento aleatorio extendido
vector<Position> randomPath(Position start, Position goal, const vector<vector<int>>& map) {
    vector<Position> path;
    Position current = start;
    path.push_back(current);

    // Continuar moviéndose hasta que el tanque llegue a la meta o no pueda moverse
    while (current.row != goal.row || current.col != goal.col) {
        Position nextMove = randomMove(current, map);

        // Si no se puede mover más, detenerse
        if (nextMove.row == current.row && nextMove.col == current.col) {
            break;
        }

        path.push_back(nextMove);
        current = nextMove;
    }

    return path;
}

// Función para decidir el tipo de movimiento según las reglas del juego
vector<Position> calculateMove(Position start, Position goal, const vector<vector<int>>& map, bool useBFS, bool useDijkstra) {
    if (useBFS) {
        cout << "Usando BFS para calcular la ruta.\n";
        return bfs(start, goal, map);
    } else if (useDijkstra) {
        cout << "Usando Dijkstra para calcular la ruta.\n";
        return dijkstra(start, goal, map);
    } else {
        cout << "Usando movimiento aleatorio.\n";
        return randomPath(start, goal, map);  // Llamar a la nueva función de movimiento aleatorio extendido
    }
}

// Función para agregar obstáculos manualmente
void addObstacles(vector<vector<int>>& map) {
    map[3][4] = 1;  // Obstáculo en la posición (3, 4)
    map[4][4] = 1;  // Obstáculo en la posición (4, 4)
    map[5][4] = 1;  // Obstáculo en la posición (5, 4)
    map[2][8] = 1; 
}

// Función para imprimir el mapa con la ruta
void printMapWithPath(const vector<vector<int>>& map, const vector<Position>& path, Position start, Position goal) {
    vector<vector<char>> visualMap(ROWS, vector<char>(COLS, '.'));

    // Colocar obstáculos en el mapa
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (map[i][j] == 1) {
                visualMap[i][j] = '#';
            }
        }
    }

    // Marcar el inicio y el objetivo
    visualMap[start.row][start.col] = 'S';  // Start
    visualMap[goal.row][goal.col] = 'G';    // Goal

    // Marcar la ruta en el mapa
    for (const Position& pos : path) {
        if (visualMap[pos.row][pos.col] != 'S' && visualMap[pos.row][pos.col] != 'G') {
            visualMap[pos.row][pos.col] = '*';  // Parte de la ruta
        }
    }

    // Imprimir el mapa
    for (const auto& row : visualMap) {
        for (char cell : row) {
            cout << cell << ' ';
        }
        cout << '\n';
    }
}

int main() {
    srand(time(0));  // Semilla para el movimiento aleatorio

    // Mapa de ejemplo (0 = libre, 1 = obstáculo)
    vector<vector<int>> map(ROWS, vector<int>(COLS, 0));

    // Agregar obstáculos manualmente
    addObstacles(map);

    Position start = {0, 0};  // Posición inicial del tanque
    Position goal = {8, 7};   // Posición objetivo

    // Simulación de las probabilidades de movimiento
    bool useBFS = rand() % 100 < 50;       // 50% de probabilidad de usar BFS
    bool useDijkstra = rand() % 100 < 80;  // 80% de probabilidad de usar Dijkstra

    vector<Position> path_lightBlue_blue = calculateMove(start, goal, map, useBFS, 0);
    vector<Position> path_yellow_red = calculateMove(start, goal, map, 0, useDijkstra);

    if (!path_lightBlue_blue.empty()) {
        cout << "Ruta encontrada:\n";
        for (const Position& pos : path_lightBlue_blue) {
            cout << "(" << pos.row << ", " << pos.col << ")\n";
        }
    } else {
        cout << "No se encontró una ruta válida.\n";
    }
 

    // Imprimir el mapa con la ruta
    cout << "Mapa tanques azul y celeste" << endl;
    printMapWithPath(map, path_lightBlue_blue, start, goal);
    cout << "Mapa tanques amarillo y rojo" << endl;
    printMapWithPath(map, path_yellow_red, start, goal);

    return 0;
}
