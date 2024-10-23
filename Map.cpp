#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>  // Para cálculos de trayectoria

// Constantes del juego
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 360;
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
const SDL_Color COLOR_BULLET = {255, 255, 0, 255};         // Amarillo para la bala

// Daño por tipo de tanque
const float DAMAGE_AZUL_CELESTE = 0.25f;  // Daño 25%
const float DAMAGE_NARANJA_ROJO = 0.50f;  // Daño 50%

// Clase para gestionar el tablero gráfico
class GameBoard {
private:
	std::vector<std::vector<char>> board;
	std::pair<int, int> selectedTank; // Coordenadas del tanque seleccionado
	bool isPlayer1Turn;  // Controla de quién es el turno
	std::vector<std::pair<int, int>> bulletPath;  // Trayectoria de la bala
	
public:
	// Constructor que inicializa el tablero
	GameBoard() : selectedTank({-1, -1}), isPlayer1Turn(true) {
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
		
		// Dibujar la trayectoria de la bala si existe
		if (!bulletPath.empty()) {
			setRenderColor(renderer, COLOR_BULLET);  // Color de la bala
			for (auto& point : bulletPath) {
				SDL_Rect bulletRect = { point.second * TILE_SIZE, point.first * TILE_SIZE, TILE_SIZE, TILE_SIZE };
				SDL_RenderFillRect(renderer, &bulletRect);  // Dibujar la bala en su trayectoria
			}
		}
	}
	
	// Cambiar el color del renderer
	void setRenderColor(SDL_Renderer* renderer, SDL_Color color) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}
	
	// Seleccionar un tanque basado en las coordenadas del clic del ratón
	bool selectTank(int mouseX, int mouseY) {
		int row = mouseY / TILE_SIZE;
		int col = mouseX / TILE_SIZE;
		if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
			// Verificar si es el turno correcto
			if (isPlayer1Turn && (board[row][col] == 'A' || board[row][col] == 'B')) {
				selectedTank = {row, col};  // Seleccionar tanque del Jugador 1
				return true;
			} else if (!isPlayer1Turn && (board[row][col] == 'C' || board[row][col] == 'D')) {
				selectedTank = {row, col};  // Seleccionar tanque del Jugador 2
				return true;
			}
		}
		return false;
	}
	
	// Mover el tanque seleccionado a la casilla destino
	bool moveSelectedTank(int mouseX, int mouseY) {
		if (selectedTank.first == -1 || selectedTank.second == -1) {
			return false;  // No hay tanque seleccionado
		}
		
		int destRow = mouseY / TILE_SIZE;
		int destCol = mouseX / TILE_SIZE;
		
		// Verificar si el movimiento es válido (adjacente y la casilla está vacía)
		int dx = abs(destRow - selectedTank.first);
		int dy = abs(destCol - selectedTank.second);
		
		if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {  // Movimiento adyacente
			if (destRow >= 0 && destRow < ROWS && destCol >= 0 && destCol < COLS && board[destRow][destCol] == '.') {
				// Mover el tanque
				board[destRow][destCol] = board[selectedTank.first][selectedTank.second];
				board[selectedTank.first][selectedTank.second] = '.';
				selectedTank = {-1, -1};  // Deseleccionar el tanque después de moverlo
				isPlayer1Turn = !isPlayer1Turn;  // Alternar turno
				bulletPath.clear();  // Limpiar trayectoria de la bala
				return true;
			}
		}
		return false;  // Movimiento no válido
	}
	
	// Disparar desde el tanque seleccionado hacia un punto de destino
	void shoot(int mouseX, int mouseY) {
		if (selectedTank.first == -1 || selectedTank.second == -1) return;  // No hay tanque seleccionado
		
		int destRow = mouseY / TILE_SIZE;
		int destCol = mouseX / TILE_SIZE;
		
		// Limpiar trayectoria anterior
		bulletPath.clear();
		
		// Aplicar línea de vista (algoritmo de Bresenham para trazar línea)
		int x1 = selectedTank.second;
		int y1 = selectedTank.first;
		int x2 = destCol;
		int y2 = destRow;
		
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);
		int sx = (x1 < x2) ? 1 : -1;
		int sy = (y1 < y2) ? 1 : -1;
		int err = dx - dy;
		
		while (true) {
			bulletPath.push_back({y1, x1});
			
			if (x1 == x2 && y1 == y2) break;
			
			int e2 = 2 * err;
			if (e2 > -dy) {
				err -= dy;
				x1 += sx;
			}
			if (e2 < dx) {
				err += dx;
				y1 += sy;
			}
			
			// Verificar si golpea un obstáculo
			if (board[y1][x1] == 'O') break;
			
			// Verificar si golpea un tanque enemigo
			if ((board[y1][x1] == 'A' || board[y1][x1] == 'B' || board[y1][x1] == 'C' || board[y1][x1] == 'D') && !(y1 == selectedTank.first && x1 == selectedTank.second)) {
				applyDamage(y1, x1);
				break;
			}
			
			// Verificar si golpea una pared y hacer rebote (implementa tu ángulo de rebote aquí)
			if (x1 <= 0 || x1 >= COLS - 1 || y1 <= 0 || y1 >= ROWS - 1) {
				// Implementa rebote aquí
				break;  // Por simplicidad, detiene el disparo al golpear una pared
			}
		}
	}
	
	// Aplicar daño a un tanque
	void applyDamage(int row, int col) {
		if (board[row][col] == 'A' || board[row][col] == 'B') {
			std::cout << "Tanque azul/celeste recibió " << DAMAGE_AZUL_CELESTE * 100 << "% de daño.\n";
		} else if (board[row][col] == 'C' || board[row][col] == 'D') {
			std::cout << "Tanque rojo/naranja recibió " << DAMAGE_NARANJA_ROJO * 100 << "% de daño.\n";
		}
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
				} else {
					gameBoard.shoot(mouseX, mouseY);  // Disparar
				}
			}
		}
		
		// Dibujar tablero
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Fondo blanco
		SDL_RenderClear(renderer);
		
		gameBoard.render(renderer);
		
		// Mostrar el turno
		std::string turnText = gameBoard.getIsPlayer1Turn() ? "Turno: Jugador 1" : "Turno: Jugador 2";
		renderText(renderer, font, turnText, 50, WINDOW_HEIGHT - 30);  // Posición del texto del turno
		
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
