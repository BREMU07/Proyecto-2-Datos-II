#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>  // Librería para renderizar texto
#include <iostream>
#include <vector>
#include <string>

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
const SDL_Color COLOR_TEXT = {0, 0, 0, 255};               // Negro para el texto

// Clase para gestionar el tablero gráfico
class GameBoard {
private:
	std::vector<std::vector<char>> board;
	
public:
	// Constructor que inicializa el tablero
	GameBoard() {
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
				
				SDL_RenderFillRect(renderer, &tileRect);  // Dibujar casilla
			}
		}
	}
	
	// Cambiar el color del renderer
	void setRenderColor(SDL_Renderer* renderer, SDL_Color color) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
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

// Función para renderizar el texto en pantalla
void renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y) {
	SDL_Color textColor = {0, 0, 0, 255};  // Color negro para el texto
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), textColor);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	
	SDL_Rect messageRect;
	messageRect.x = x;
	messageRect.y = y;
	messageRect.w = surfaceMessage->w;
	messageRect.h = surfaceMessage->h;
	
	SDL_RenderCopy(renderer, message, NULL, &messageRect);
	
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}

// Inicializar SDL y ejecutar el juego
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();  // Inicializar SDL_ttf
	
	SDL_Window* window = SDL_CreateWindow("Tank Attack!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	// Cargar la fuente de texto
	TTF_Font* font = TTF_OpenFont("arial.ttf", 24);  // Asegúrate de tener esta fuente o una similar
	if (!font) {
		std::cerr << "Error al cargar la fuente: " << TTF_GetError() << std::endl;
		return -1;
	}
	
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
		
		// Contar tanques vivos
		std::pair<int, int> tankCounts = gameBoard.countTanks();
		std::string player1Text = "Jugador 1: " + std::to_string(tankCounts.first) + " tanques vivos";
		std::string player2Text = "Jugador 2: " + std::to_string(tankCounts.second) + " tanques vivos";
		
		// Renderizar el texto
		renderText(renderer, font, player1Text, 50, WINDOW_HEIGHT - 30);  // Posición del texto del Jugador 1
		renderText(renderer, font, player2Text, 350, WINDOW_HEIGHT - 30); // Posición del texto del Jugador 2
		
		SDL_RenderPresent(renderer);  // Mostrar en pantalla
	}
	
	// Limpiar y cerrar SDL
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}


