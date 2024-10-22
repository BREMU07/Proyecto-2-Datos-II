#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include "tank_movement.cpp"

// Constantes del juego
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 360;  // Aumentado para incluir el contador
const int ROWS = 10;
const int COLS = 20;
const int TILE_SIZE = 32;
const int OBSTACLE_PERCENTAGE = 10;

// Colores para representar los objetos en el tablero
const SDL_Color COLOR_OBSTACLE = {169, 169, 169, 255};     // Gris para obstáculos
const SDL_Color COLOR_PLAYER1_TANK1 = {0, 0, 255, 255};    // Azul para Tanque 1 Jugador 1
const SDL_Color COLOR_PLAYER1_TANK2 = {0, 255, 255, 255};  // Cian para Tanque 2 Jugador 1
const SDL_Color COLOR_PLAYER2_TANK1 = {255, 0, 0, 255};    // Rojo para Tanque 1 Jugador 2
const SDL_Color COLOR_PLAYER2_TANK2 = {255, 165, 0, 255};  // Naranja para Tanque 2 Jugador 2
const SDL_Color COLOR_EMPTY = {255, 255, 224, 255};        // Amarillo claro para espacios vacíos
const SDL_Color COLOR_SELECTED = {255, 255, 0, 255};       // Amarillo para el tanque seleccionado

TankMovement tankMovement(ROWS, COLS);

// Clase para gestionar el tablero gráfico
class GameBoard {
private:
	std::vector<std::vector<char>> board;
	std::pair<int, int> selectedTank; // Coordenadas del tanque seleccionado
	
public:
	// Constructor que inicializa el tablero
	GameBoard() : selectedTank({-1, -1}) {
		board.resize(ROWS, std::vector<char>(COLS, '.'));
		generateObstacles();
		placeTanksInCorners('A', 'B', true);  // Jugador 1: Tanques A y B
		placeTanksInCorners('C', 'D', false); // Jugador 2: Tanques C y D
	}
	
	// Generar obstáculos aleatorios en el tablero
	void generateObstacles() {
		srand(SDL_GetTicks());  // Semilla aleatoria
		for (int i = 2; i < ROWS - 2; ++i) {
			for (int j = 2; j < COLS - 2; ++j) {
				if (rand() % 100 < OBSTACLE_PERCENTAGE) {
					board[i][j] = 'O';  // Obstáculo
				}
			}
		}
	}
	
	// Colocar tanques en las esquinas del tablero
	void placeTanksInCorners(char tankType1, char tankType2, bool topHalf) {
		if (topHalf) {
			board[0][0] = tankType1; board[0][1] = tankType1;
			board[0][COLS-2] = tankType2; board[0][COLS-1] = tankType2;
		} else {
			board[ROWS-1][0] = tankType1; board[ROWS-1][1] = tankType1;
			board[ROWS-1][COLS-2] = tankType2; board[ROWS-1][COLS-1] = tankType2;
		}
	}
	
	// Dibujar el tablero en pantalla usando SDL2
	void render(SDL_Renderer* renderer) {
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLS; ++j) {
				SDL_Rect tileRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
				
				if (board[i][j] == 'O') {
					setRenderColor(renderer, COLOR_OBSTACLE);  // Obstáculos gris
				} else if (board[i][j] == 'A') {
					setRenderColor(renderer, COLOR_PLAYER1_TANK1);  // Jugador 1, tanque A: azul
				} else if (board[i][j] == 'B') {
					setRenderColor(renderer, COLOR_PLAYER1_TANK2);  // Jugador 1, tanque B: cian
				} else if (board[i][j] == 'C') {
					setRenderColor(renderer, COLOR_PLAYER2_TANK1);  // Jugador 2, tanque C: rojo
				} else if (board[i][j] == 'D') {
					setRenderColor(renderer, COLOR_PLAYER2_TANK2);  // Jugador 2, tanque D: naranja
				} else {
					setRenderColor(renderer, COLOR_EMPTY);  // Casillas vacías: amarillo claro
				}
				
				// Si la casilla es el tanque seleccionado
				if (selectedTank.first == i && selectedTank.second == j) {
					setRenderColor(renderer, COLOR_SELECTED);  // Color para el tanque seleccionado
				}
				
				SDL_RenderFillRect(renderer, &tileRect);  // Dibujar casilla
			}
		}
	}

	// Cambiar el color del renderer
	void setRenderColor(SDL_Renderer* renderer, SDL_Color color) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}

	std::vector<std::vector<int>> toIntMap() {
		std::vector<std::vector<int>> intMap(ROWS, std::vector<int>(COLS, 0));  // Inicializar con 0 (espacios vacíos)
		
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLS; ++j) {
				if (board[i][j] == 'O') {
					intMap[i][j] = 1;  // Obstáculos
				} else if (board[i][j] == 'A' || board[i][j] == 'B') {
					intMap[i][j] = 2;  // Tanques de Jugador 1
				} else if (board[i][j] == 'C' || board[i][j] == 'D') {
					intMap[i][j] = 3;  // Tanques de Jugador 2
				}
			}
		}
		
		return intMap;
	}
	
	// Seleccionar un tanque basado en las coordenadas del clic del ratón
	bool selectTank(int mouseX, int mouseY) {
		int row = mouseY / TILE_SIZE;
		int col = mouseX / TILE_SIZE;
		if (row >= 0 && row < ROWS && col >= 0 && col < COLS &&
			(board[row][col] == 'A' || board[row][col] == 'B' ||
				board[row][col] == 'C' || board[row][col] == 'D')) {
			selectedTank = {row, col};  // Guardar las coordenadas del tanque seleccionado
			cout << selectedTank.first << endl;
			return true;
		}
				return false;
	}
	
	// Mover el tanque seleccionado a la casilla destino
	bool moveSelectedTank(int mouseX, int mouseY) {
		GameBoard gameBoard;

		std::vector<std::vector<int>> intMap = gameBoard.toIntMap();

		if (selectedTank.first == -1 || selectedTank.second == -1) {
			return false;  // No hay tanque seleccionado
		}
		
		int destRow = mouseY / TILE_SIZE;
		int destCol = mouseX / TILE_SIZE;
		
		// Verificar si el movimiento es válido (adjacente y la casilla está vacía)
		int dx = abs(destRow - selectedTank.first);
		int dy = abs(destCol - selectedTank.second);

		TankMovement::Position start = {selectedTank.first, selectedTank.second};
		TankMovement::Position goal = {destRow, destCol};

		bool useBFS = rand() % 100 < 50;
    	bool useDijkstra = rand() % 100 < 80;

		if (board[selectedTank.first][selectedTank.second] == 'A' || board[selectedTank.first][selectedTank.second] == 'C') {
			vector<TankMovement::Position> path_lightblue_blue = tankMovement.calculateMove(start, goal, intMap, useBFS, 0);
			tankMovement.printMapWithPath(intMap, path_lightblue_blue, start, goal);
		}
		else if (board[selectedTank.first][selectedTank.second] == 'B' || board[selectedTank.first][selectedTank.second] == 'D') {
			vector<TankMovement::Position> path_yellow_red = tankMovement.calculateMove(start, goal, intMap, 0, useDijkstra);
			tankMovement.printMapWithPath(intMap, path_yellow_red, start, goal);
		}


		if (destRow >= 0 && destRow < ROWS && destCol >= 0 && destCol < COLS && board[destRow][destCol] == '.') {
			// Mover el tanque
			board[destRow][destCol] = board[selectedTank.first][selectedTank.second];
			board[selectedTank.first][selectedTank.second] = '.';
			selectedTank = {-1, -1};  // Deseleccionar el tanque después de moverlo
			return true;
		}
		
		return false;  // Movimiento no válido
	}
	
	// Contar tanques vivos de cada jugador
	std::pair<int, int> countTanks() {
		int player1Tanks = 0;
		int player2Tanks = 0;
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLS; ++j) {
				if (board[i][j] == 'A' || board[i][j] == 'B') {
					player1Tanks++;
				} else if (board[i][j] == 'C' || board[i][j] == 'D') {
					player2Tanks++;
				}
			}
		}
		return {player1Tanks, player2Tanks};  // Retorna un par con los tanques de cada jugador
	}
};

// Inicializar SDL y ejecutar el juego
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Window* window = SDL_CreateWindow("Tank Attack!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	GameBoard gameBoard;

    //std::vector<std::vector<int>> intMap = gameBoard.toIntMap();
    //vector<TankMovement::Position> path_lightblue_blue = tankMovement.calculateMove(start, goal, intMap, useBFS, 0);
    //tankMovement.printMapWithPath(intMap, path_lightblue_blue, start, goal);
    
	bool running = true;
	SDL_Event event;
	
	while (running) {
			// Manejo de eventos (cerrar ventana, clics del ratón)
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = false;
				}
				else if (event.type == SDL_MOUSEBUTTONDOWN) {
					int mouseX = event.button.x;
					int mouseY = event.button.y;
					
					if (!gameBoard.selectTank(mouseX, mouseY)) {
						gameBoard.moveSelectedTank(mouseX, mouseY);  // Si no selecciona tanque, intenta moverlo
					}
				}
			}
			
			// Dibujar tablero
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Fondo blanco
			SDL_RenderClear(renderer);
			
			gameBoard.render(renderer);
			
			SDL_RenderPresent(renderer);  // Mostrar en pantalla
		}
		
		// Limpiar y cerrar SDL
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		
		return 0;
}