#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int ROWS = 10;  // N�mero de filas del tablero
const int COLS = 20;  // N�mero de columnas del tablero
const int OBSTACLE_PERCENTAGE = 10;  // Porcentaje reducido de obst�culos

class GameBoard {
private:
	std::vector<std::vector<char>> board;
	
public:
	// Constructor que inicializa el tablero
	GameBoard() {
		// Inicializar la matriz con espacios vac�os ('.')
		board.resize(ROWS, std::vector<char>(COLS, '.'));
		
		// Generar obst�culos en el centro del tablero
		generateObstacles();
		
		// Colocar tanques del Jugador 1 en las esquinas superiores
		placeTanksInCorners('A', 'B', true);  // 'A' y 'B' representan los tanques del Jugador 1
		
		// Colocar tanques del Jugador 2 en las esquinas inferiores
		placeTanksInCorners('C', 'D', false);  // 'C' y 'D' representan los tanques del Jugador 2
	}
	
	// Generar obst�culos aleatorios
	void generateObstacles() {
		srand(time(0));  // Semilla aleatoria
		for (int i = 2; i < ROWS - 2; ++i) {
			for (int j = 2; j < COLS - 2; ++j) {
				// Colocar un obst�culo con una probabilidad del 10%
				if (rand() % 100 < OBSTACLE_PERCENTAGE) {
					board[i][j] = 'O';  // 'O' representa un obst�culo
				}
			}
		}
	}
	
	// Colocar los tanques en las esquinas del tablero
	void placeTanksInCorners(char tankType1, char tankType2, bool topHalf) {
		if (topHalf) {
			// Esquinas superiores para Jugador 1
			board[0][0] = tankType1; board[0][1] = tankType1;  // Esquina superior izquierda (2 espacios)
			board[0][COLS-2] = tankType2; board[0][COLS-1] = tankType2;  // Esquina superior derecha (2 espacios)
		} else {
			// Esquinas inferiores para Jugador 2
			board[ROWS-1][0] = tankType1; board[ROWS-1][1] = tankType1;  // Esquina inferior izquierda (2 espacios)
			board[ROWS-1][COLS-2] = tankType2; board[ROWS-1][COLS-1] = tankType2;  // Esquina inferior derecha (2 espacios)
		}
	}
	
	// Mostrar el tablero en consola
	void printBoard() {
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLS; ++j) {
				std::cout << board[i][j] << ' ';
			}
			std::cout << std::endl;
		}
	}
};

int main() {
	GameBoard gameBoard;
	std::cout << "Tablero del juego (O = Obstaculo, A/B = Tanques Jugador 1, C/D = Tanques Jugador 2, . = Espacio libre):" << std::endl;
	gameBoard.printBoard();
	
	return 0;
}
//a
