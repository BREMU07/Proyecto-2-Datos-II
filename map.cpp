#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath> 
#include <ctime> // Para cálculos de trayectoria
#include "tank_movement.cpp"
#include "power_up.cpp"

// Constantes del juego
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 360;
const int ROWS = 10;
const int COLS = 20;
const int TILE_SIZE = 32;
const int OBSTACLE_PERCENTAGE = 10;
const int MAX_GAME_DURATION = 5 * 60 * 1000;  // 5 minutos en milisegundos

// Colores para representar los objetos en el tablero
const SDL_Color COLOR_OBSTACLE = {169, 169, 169, 255};     // Gris para obstáculos
const SDL_Color COLOR_PLAYER1_TANK1 = {0, 0, 255, 255};    // Azul para Tanque A (Jugador 1 BFS)
const SDL_Color COLOR_PLAYER1_TANK2 = {0, 255, 255, 255};  // Cian para Tanque B (Jugador 1 Dijkstra)
const SDL_Color COLOR_PLAYER2_TANK1 = {255, 0, 0, 255};    // Rojo para Tanque C (Jugador 2 BFS)
const SDL_Color COLOR_PLAYER2_TANK2 = {255, 165, 0, 255};  // Naranja para Tanque D (Jugador 2 Dijkstra)
const SDL_Color COLOR_EMPTY = {255, 255, 224, 255};        // Amarillo claro para espacios vacíos
const SDL_Color COLOR_SELECTED = {255, 255, 0, 255};       // Amarillo para el tanque seleccionado
const SDL_Color COLOR_BULLET = {255, 255, 0, 255};         // Amarillo para la bala

// Daño por tipo de tanque
const float DAMAGE_AZUL_CELESTE = 0.25f;  // Daño 25%
const float DAMAGE_NARANJA_ROJO = 0.50f;  // Daño 50%

// Clase que gestiona el movimiento del tanque
TankMovement tankMovement(ROWS, COLS);

// Estructura para definir un tanque
struct Tank {
    char type;  // Tipo de tanque (A, B, C, D)
    int row, col;  // Posición del tanque
    float health;  // Vida del tanque

    Tank(char t, int r, int c, float h) : type(t), row(r), col(c), health(h) {}
};

// Clase para gestionar el tablero gráfico
class GameBoard {
private:
    std::vector<std::vector<char>> board;
    std::pair<int, int> selectedTank;       // Coordenadas del tanque seleccionado
    std::pair<int, int> shootingTarget;     // Casilla objetivo del disparo
    bool isPlayer1Turn;                     // Controla de quién es el turno
    bool isShootingPreview;                 // Indica si se está mostrando la trayectoria
    std::vector<std::pair<int, int>> bulletPath;  // Trayectoria de la bala
    std::vector<Tank> tanks;  // Lista de tanques en el juego
    Uint32 startTime;  // Tiempo de inicio del juego
    std::vector<PowerUp> powerUps;  // Lista de Power-Ups

    // Estados de power-up
    bool extraTurn = false;
    bool movementPrecision = false;
    bool attackPrecision = false;
    bool attackPower = false;

public:
    // Constructor que inicializa el tablero
    GameBoard() : selectedTank({-1, -1}), shootingTarget({-1, -1}), isPlayer1Turn(true), isShootingPreview(false) {
        board.resize(ROWS, std::vector<char>(COLS, '.'));
        generateObstacles();  // Generamos los obstáculos al inicio
        generatePowerUps(powerUps, ROWS, COLS);  // Generar power-ups
        // Colocamos 8 tanques y asignamos vida
        tanks.push_back(Tank('A', 0, 0, 100));  // Tanque A, Jugador 1 (BFS)
        tanks.push_back(Tank('B', 0, 1, 100));  // Tanque B, Jugador 1 (Dijkstra)
        tanks.push_back(Tank('A', 0, COLS - 2, 100));  // Tanque A, Jugador 1 (BFS)
        tanks.push_back(Tank('B', 0, COLS - 1, 100));  // Tanque B, Jugador 1 (Dijkstra)

        tanks.push_back(Tank('C', ROWS - 1, 0, 100));  // Tanque C, Jugador 2 (BFS)
        tanks.push_back(Tank('D', ROWS - 1, 1, 100));  // Tanque D, Jugador 2 (Dijkstra)
        tanks.push_back(Tank('C', ROWS - 1, COLS - 2, 100));  // Tanque C, Jugador 2 (BFS)
        tanks.push_back(Tank('D', ROWS - 1, COLS - 1, 100));  // Tanque D, Jugador 2 (Dijkstra)

        startTime = SDL_GetTicks();  // Inicializamos el tiempo del juego
        updateBoard();
    }

    bool getIsShootingPreview() { return isShootingPreview; }

    // Función para actualizar la posición de los tanques en el tablero sin eliminar obstáculos
    void updateBoard() {
        // Limpiamos las posiciones de tanques en el tablero, sin tocar los obstáculos
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (board[i][j] != 'O') {  // No tocamos las casillas con obstáculos
                    board[i][j] = '.';
                }
            }
        }
        // Actualizamos la posición de los tanques
        for (auto &tank : tanks) {
            if (tank.health > 0) {
                board[tank.row][tank.col] = tank.type;  // Solo actualizamos tanques con vida
            }
        }
    }

    // Función para contar cuántos tanques tiene un jugador vivo
    int countTanks(bool player1) {
        int count = 0;
        for (const auto& tank : tanks) {
            if (tank.health > 0 && ((player1 && (tank.type == 'A' || tank.type == 'B')) ||
                                     (!player1 && (tank.type == 'C' || tank.type == 'D')))) {
                ++count;
            }
        }
        return count;
    }

    // Función para determinar el ganador y mostrarlo en la consola
	void printWinner() {
		int player1Tanks = countTanks(true);
		int player2Tanks = countTanks(false);
		if (player1Tanks == 0) {
			std::cout << "Jugador 2 gana!" << std::endl;
		} else if (player2Tanks == 0) {
			std::cout << "Jugador 1 gana!" << std::endl;
		} else if (SDL_GetTicks() - startTime >= MAX_GAME_DURATION) {
			if (player1Tanks > player2Tanks) {
				std::cout << "Jugador 1 gana por más tanques!" << std::endl;
			} else {
				std::cout << "Jugador 2 gana por más tanques!" << std::endl;
			}
		}
	}

    // Función para aplicar daño a un tanque
    void applyDamage(int row, int col, float damage) {
        for (auto &tank : tanks) {
            if (tank.row == row && tank.col == col && tank.health > 0) {
                tank.health -= damage;
                std::cout << "Tanque " << tank.type << " recibió " << damage << "% de daño. Vida restante: " << tank.health << "\n";
                if (tank.health <= 0) {
                    board[tank.row][tank.col] = '.';  // Eliminar tanque del tablero
                    std::cout << "Tanque " << tank.type << " ha sido destruido.\n";
                }
                break;
            }
        }
        updateBoard();  // Actualizamos el tablero después del daño
    }

    // Función para disparar la bala si el jugador hace clic derecho nuevamente en la misma casilla
    void shoot() {
        if (isShootingPreview && shootingTarget.first != -1 && shootingTarget.second != -1) {
            if (attackPower) {
                applyDamage(shootingTarget.first, shootingTarget.second, 100.0f);  // Daño máximo
                attackPower = false;
            } else {
                float damage = (selectedTank.first == 'A' || selectedTank.first == 'C') ? DAMAGE_AZUL_CELESTE : DAMAGE_NARANJA_ROJO;
                applyDamage(shootingTarget.first, shootingTarget.second, damage * 100);  // Daño normal
            }

            isShootingPreview = false;
            shootingTarget = {-1, -1};
            bulletPath.clear();

            // Cambiar turno, salvo si hay power-up de doble turno
            if (!extraTurn) {
                isPlayer1Turn = !isPlayer1Turn;
            } else {
                extraTurn = false;
            }
        }
    }

    // Mostrar la trayectoria para el disparo
    void previewShoot(int mouseX, int mouseY) {
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

            // Verificar si golpea un obstáculo o un tanque enemigo
            if (board[y1][x1] == 'O' || (board[y1][x1] != '.' && (y1 != selectedTank.first || x1 != selectedTank.second))) {
                break;
            }
        }

        // Guardar la casilla objetivo
        shootingTarget = {destRow, destCol};
        isShootingPreview = true;
    }

    // Dibujar el tablero en pantalla usando SDL2
    void render(SDL_Renderer* renderer) {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                SDL_Rect tileRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };

                if (board[i][j] == 'O') {
                    setRenderColor(renderer, COLOR_OBSTACLE);  // Obstáculos gris
                } else if (board[i][j] == 'A' || board[i][j] == 'B' || board[i][j] == 'C' || board[i][j] == 'D') {
                    // Si el tanque está vivo, lo dibujamos
                    for (auto &tank : tanks) {
                        if (tank.row == i && tank.col == j && tank.health > 0) {
                            if (tank.type == 'A') setRenderColor(renderer, COLOR_PLAYER1_TANK1);  // Azul para tanques A (Jugador 1 BFS)
                            else if (tank.type == 'B') setRenderColor(renderer, COLOR_PLAYER1_TANK2);  // Cian para tanques B (Jugador 1 Dijkstra)
                            else if (tank.type == 'C') setRenderColor(renderer, COLOR_PLAYER2_TANK1);  // Rojo para tanques C (Jugador 2 BFS)
                            else if (tank.type == 'D') setRenderColor(renderer, COLOR_PLAYER2_TANK2);  // Naranja para tanques D (Jugador 2 Dijkstra)
                            break;
                        }
                    }
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

        // Renderizar los power-ups en el tablero
        for (auto& powerUp : powerUps) {
            powerUp.render(renderer);
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

    // Hacer públicas las funciones selectTank y moveSelectedTank
    bool selectTank(int mouseX, int mouseY) {
        int row = mouseY / TILE_SIZE;
        int col = mouseX / TILE_SIZE;
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            if (isPlayer1Turn && (board[row][col] == 'A' || board[row][col] == 'B')) {
                selectedTank = {row, col};
                return true;
            } else if (!isPlayer1Turn && (board[row][col] == 'C' || board[row][col] == 'D')) {
                selectedTank = {row, col};
                return true;
            }
        }
        return false;
    }

	bool moveSelectedTank(int mouseX, int mouseY) {
		std::vector<std::vector<int>> intMap = toIntMap();
		if (selectedTank.first == -1 || selectedTank.second == -1) return false;

		int destRow = mouseY / TILE_SIZE;
		int destCol = mouseX / TILE_SIZE;

		TankMovement::Position start = {selectedTank.first, selectedTank.second};
		TankMovement::Position goal = {destRow, destCol};

		std::vector<TankMovement::Position> path;
		bool useBFS = rand() % 100 < 50;
		bool useDijkstra = rand() % 100 < 80;

		if (board[selectedTank.first][selectedTank.second] == 'A' || board[selectedTank.first][selectedTank.second] == 'C') {
			path = tankMovement.calculateMove(start, goal, intMap, useBFS, 0);
			tankMovement.printMapWithPath(intMap, path, start, goal);
		} else if (board[selectedTank.first][selectedTank.second] == 'B' || board[selectedTank.first][selectedTank.second] == 'D') {
			path = tankMovement.calculateMove(start, goal, intMap, 0, useDijkstra);
			tankMovement.printMapWithPath(intMap, path, start, goal);
		}

		if (!path.empty()) {
			TankMovement::Position newPos = path.back();

			// Comprobar si el tanque colisiona con un Power-Up
			bool powerUpCollected = checkPowerUpCollision(newPos.row, newPos.col);
			
			// Actualizar la posici贸n del tanque en el tablero
			board[newPos.row][newPos.col] = board[selectedTank.first][selectedTank.second];  // Mover el tanque a la nueva posici贸n
			board[selectedTank.first][selectedTank.second] = '.';  // Limpiar la casilla anterior

			// Actualizar el tanque en la lista de tanques
			for (auto &tank : tanks) {
				if (tank.row == selectedTank.first && tank.col == selectedTank.second) {
					tank.row = newPos.row;
					tank.col = newPos.col;
					break;
				}
			}

			// Deseleccionar el tanque y cambiar el turno
			selectedTank = {-1, -1};

			// Si se recogi贸 un power-up, aplicar sus efectos.
			if (powerUpCollected) {
				std::cout << "Power-Up recogido!\n";
			}

			// Cambiar turno
			isPlayer1Turn = !isPlayer1Turn;
			updateBoard();  // Actualizar el tablero con la nueva posici贸n

			return true;
		}

		return false;
	}

    bool checkPowerUpCollision(int tankRow, int tankCol) {
        for (auto it = powerUps.begin(); it != powerUps.end(); ++it) {
            if (it->row == tankRow && it->col == tankCol) {
                it->applyPowerUp(extraTurn, movementPrecision, attackPrecision, attackPower);
                powerUps.erase(it);
                return true;
            }
        }
        return false;
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
};

// Inicializar SDL y ejecutar el juego
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();  // Inicializar SDL_ttf

    SDL_Window* window = SDL_CreateWindow("Tank Attack!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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

                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Clic izquierdo: Mover tanque
                    if (!gameBoard.selectTank(mouseX, mouseY)) {
                        gameBoard.moveSelectedTank(mouseX, mouseY);  // Si no selecciona tanque, intenta moverlo
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    // Clic derecho: Ver o confirmar disparo
                    if (gameBoard.getIsShootingPreview()) {
                        gameBoard.shoot();  // Si ya está en modo de previsualización, dispara
                    } else {
                        gameBoard.previewShoot(mouseX, mouseY);  // Previsualiza la trayectoria
                    }
                }
            }
        }

        // Dibujar tablero
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Fondo blanco
        SDL_RenderClear(renderer);

        gameBoard.render(renderer);

        SDL_RenderPresent(renderer);  // Mostrar en pantalla

        // Determinar si el juego terminó por tiempo o por destrucción de tanques
        gameBoard.printWinner();  // Imprimir el ganador en la consola si lo hay
    }

    // Limpiar y cerrar SDL
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}