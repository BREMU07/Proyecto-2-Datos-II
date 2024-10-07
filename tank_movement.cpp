#include <iostream>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <algorithm>  // Necesario para std::reverse

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
bool isValidMove(int row, int col, const std::vector<std::vector<int>>& map) {
    return (row >= 0 && row < ROWS && col >= 0 && col < COLS && map[row][col] == 0);  // Verifica que no haya obstáculo
}

// Implementación de BFS para encontrar el camino más corto
std::vector<Position> bfs(Position start, Position goal, const std::vector<std::vector<int>>& map) {
    std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));
    std::vector<std::vector<Position>> previous(ROWS, std::vector<Position>(COLS, {-1, -1}));
    std::queue<Position> q;

    q.push(start);
    visited[start.row][start.col] = true;

    while (!q.empty()) {
        Position current = q.front();
        q.pop();

        if (current.row == goal.row && current.col == goal.col) {
            // Se ha encontrado la meta, reconstruir la ruta
            std::vector<Position> path;
            for (Position at = goal; at.row != -1; at = previous[at.row][at.col]) {
                path.push_back(at);
            }
            std::reverse(path.begin(), path.end());  // Revertir la ruta para obtener el camino correcto
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
std::vector<Position> dijkstra(Position start, Position goal, const std::vector<std::vector<int>>& map) {
    std::vector<std::vector<int>> dist(ROWS, std::vector<int>(COLS, INT_MAX));
    std::vector<std::vector<Position>> previous(ROWS, std::vector<Position>(COLS, {-1, -1}));
    std::priority_queue<std::pair<int, Position>, std::vector<std::pair<int, Position>>, std::greater<>> pq;

    pq.push({0, start});
    dist[start.row][start.col] = 0;

    while (!pq.empty()) {
        Position current = pq.top().second;
        int currentDist = pq.top().first;
        pq.pop();

        if (current.row == goal.row && current.col == goal.col) {
            // Se ha encontrado la meta, reconstruir la ruta
            std::vector<Position> path;
            for (Position at = goal; at.row != -1; at = previous[at.row][at.col]) {
                path.push_back(at);
            }
            std::reverse(path.begin(), path.end());  // Revertir la ruta
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
Position randomMove(Position current, const std::vector<std::vector<int>>& map) {
    std::vector<Position> validMoves;

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
std::vector<Position> randomPath(Position start, Position goal, const std::vector<std::vector<int>>& map) {
    std::vector<Position> path;
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
std::vector<Position> calculateMove(Position start, Position goal, const std::vector<std::vector<int>>& map, bool useBFS, bool useDijkstra) {
    if (useBFS) {
        std::cout << "Usando BFS para calcular la ruta.\n";
        return bfs(start, goal, map);
    } else if (useDijkstra) {
        std::cout << "Usando Dijkstra para calcular la ruta.\n";
        return dijkstra(start, goal, map);
    } else {
        std::cout << "Usando movimiento aleatorio.\n";
        return randomPath(start, goal, map);  // Llamar a la nueva función de movimiento aleatorio extendido
    }
}

// Función para agregar obstáculos manualmente
void addObstacles(std::vector<std::vector<int>>& map) {
    map[3][4] = 1;  // Obstáculo en la posición (3, 4)
    map[4][4] = 1;  // Obstáculo en la posición (4, 4)
    map[5][4] = 1;  // Obstáculo en la posición (5, 4)
    map[2][8] = 1; 
}

// Función para imprimir el mapa con la ruta
void printMapWithPath(const std::vector<std::vector<int>>& map, const std::vector<Position>& path, Position start, Position goal) {
    std::vector<std::vector<char>> visualMap(ROWS, std::vector<char>(COLS, '.'));

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
    std::cout << "\nMapa con la ruta:\n";
    for (const auto& row : visualMap) {
        for (char cell : row) {
            std::cout << cell << ' ';
        }
        std::cout << '\n';
    }
}

int main() {
    srand(time(0));  // Semilla para el movimiento aleatorio

    // Mapa de ejemplo (0 = libre, 1 = obstáculo)
    std::vector<std::vector<int>> map(ROWS, std::vector<int>(COLS, 0));

    // Agregar obstáculos manualmente
    addObstacles(map);

    Position start = {0, 0};  // Posición inicial del tanque
    Position goal = {8, 7};   // Posición objetivo

    // Simulación de las probabilidades de movimiento
    bool useBFS = rand() % 100 < 50;       // 50% de probabilidad de usar BFS
    bool useDijkstra = rand() % 100 < 80;  // 80% de probabilidad de usar Dijkstra

    std::vector<Position> path = calculateMove(start, goal, map, useBFS, useDijkstra);

    if (!path.empty()) {
        std::cout << "Ruta encontrada:\n";
        for (const Position& pos : path) {
            std::cout << "(" << pos.row << ", " << pos.col << ")\n";
        }
    } else {
        std::cout << "No se encontró una ruta válida.\n";
    }

    // Imprimir el mapa con la ruta
    printMapWithPath(map, path, start, goal);

    return 0;
}
