#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

// Constantes del juego
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;
const int ROWS = 10;
const int COLS = 20;
const int TILE_SIZE = 32;
const int OBSTACLE_PERCENTAGE = 10;

// Colores para representar los objetos en el tablero
const SDL_Color COLOR_OBSTACLE = {0, 0, 0, 255};     // Negro para obstáculos
const SDL_Color COLOR_PLAYER1 = {0, 0, 255, 255};    // Azul para Jugador 1
const SDL_Color COLOR_PLAYER2 = {255, 0, 0, 255};    // Rojo para Jugador 2
const SDL_Color COLOR_EMPTY = {255, 255, 255, 255};  // Blanco para espacios vacíos

// Clase para gestionar el tablero gráfico
class GameBoard {
private:
	std::vector<std::vector<char>> board;
	
public:
	// Constructor que inicializa el tablero
	GameBoard() {
		board.resize(ROWS, std::vector<char>(COLS, '.'));
		generateObstacles();
		placeTanksInCorners('A', 'B', true);
		placeTanksInCorners('C', 'D', false);
	}
	
	// Generar obstáculos aleatorios en el tablero
	void generateObstacles() {
		srand(SDL_GetTicks());  // Semilla aleatoria
		for (int i = 2; i < ROWS - 2; ++i) {
			for (int j = 2; j < COLS - 2; ++j) {
				if (rand() % 100 < OBSTACLE_PERCENTAGE) {
					board[i][j] = 'O';
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
					setRenderColor(renderer, COLOR_OBSTACLE);
				} else if (board[i][j] == 'A' || board[i][j] == 'B') {
					setRenderColor(renderer, COLOR_PLAYER1);
				} else if (board[i][j] == 'C' || board[i][j] == 'D') {
					setRenderColor(renderer, COLOR_PLAYER2);
				} else {
					setRenderColor(renderer, COLOR_EMPTY);
				}
				
				SDL_RenderFillRect(renderer, &tileRect);  // Dibujar casilla
			}
		}
	}
	
	// Cambiar el color del renderer
	void setRenderColor(SDL_Renderer* renderer, SDL_Color color) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}
};

// Inicializar SDL y ejecutar el juego
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Window* window = SDL_CreateWindow("Tank Attack!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	GameBoard gameBoard;
	
	bool running = true;
	SDL_Event event;
	
	while (running) {
		// Manejo de eventos (cerrar ventana)
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
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
