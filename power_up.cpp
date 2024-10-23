#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

// Colores del Power-Up
const SDL_Color COLOR_DOUBLE_TURN = {255, 215, 0, 255};  // Dorado
const SDL_Color COLOR_MOVEMENT_PRECISION = {0, 255, 127, 255};  // Verde
const SDL_Color COLOR_ATTACK_PRECISION = {255, 69, 0, 255};  // Rojo
const SDL_Color COLOR_ATTACK_POWER = {138, 43, 226, 255};  // Violeta
extern const int TILE_SIZE;

// Enumeraci贸n de tipos de Power-Up
enum PowerUpType {
    DOUBLE_TURN,
    MOVEMENT_PRECISION,
    ATTACK_PRECISION,
    ATTACK_POWER
};

// Clase para representar un Power-Up
class PowerUp {
public:
    PowerUpType type;
    int row, col;  // Posici贸n en el tablero

    PowerUp(PowerUpType t, int r, int c) : type(t), row(r), col(c) {}

    // Renderizar el Power-Up en el tablero
    void render(SDL_Renderer* renderer) {
        SDL_Rect rect = { col * 32, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        switch (type) {
            case DOUBLE_TURN:
                SDL_SetRenderDrawColor(renderer, COLOR_DOUBLE_TURN.r, COLOR_DOUBLE_TURN.g, COLOR_DOUBLE_TURN.b, COLOR_DOUBLE_TURN.a);
                break;
            case MOVEMENT_PRECISION:
                SDL_SetRenderDrawColor(renderer, COLOR_MOVEMENT_PRECISION.r, COLOR_MOVEMENT_PRECISION.g, COLOR_MOVEMENT_PRECISION.b, COLOR_MOVEMENT_PRECISION.a);
                break;
            case ATTACK_PRECISION:
                SDL_SetRenderDrawColor(renderer, COLOR_ATTACK_PRECISION.r, COLOR_ATTACK_PRECISION.g, COLOR_ATTACK_PRECISION.b, COLOR_ATTACK_PRECISION.a);
                break;
            case ATTACK_POWER:
                SDL_SetRenderDrawColor(renderer, COLOR_ATTACK_POWER.r, COLOR_ATTACK_POWER.g, COLOR_ATTACK_POWER.b, COLOR_ATTACK_POWER.a);
                break;
        }
        SDL_RenderFillRect(renderer, &rect);
    }

    // Aplicar el Power-Up
    void applyPowerUp(bool& extraTurn, bool& movementPrecision, bool& attackPrecision, bool& attackPower) {
        switch (type) {
            case DOUBLE_TURN:
                extraTurn = true;
                std::cout << "Power-Up de Doble Turno activado.\n";
                break;
            case MOVEMENT_PRECISION:
                movementPrecision = true;
                std::cout << "Power-Up de Precisi贸n de Movimiento activado.\n";
                break;
            case ATTACK_PRECISION:
                attackPrecision = true;
                std::cout << "Power-Up de Precisi贸n de Ataque activado.\n";
                break;
            case ATTACK_POWER:
                attackPower = true;
                std::cout << "Power-Up de Poder de Ataque activado.\n";
                break;
        }
    }
};

// Generar Power-Ups aleatorios en el tablero
void generatePowerUps(std::vector<PowerUp>& powerUps, int rows, int cols) {
    srand(SDL_GetTicks());
    for (int i = 0; i < 4; ++i) {
        int randomRow = rand() % (rows - 4) + 2;  // Evitamos bordes
        int randomCol = rand() % (cols - 4) + 2;
        PowerUpType type = static_cast<PowerUpType>(rand() % 4);  // Generar tipo aleatorio
        powerUps.push_back(PowerUp(type, randomRow, randomCol));
    }
}

// Verificar si el tanque colisiona con un Power-Up
bool checkPowerUpCollision(std::vector<PowerUp>& powerUps, int tankRow, int tankCol, bool& extraTurn, bool& movementPrecision, bool& attackPrecision, bool& attackPower) {
    for (auto it = powerUps.begin(); it != powerUps.end(); ++it) {
        if (it->row == tankRow && it->col == tankCol) {
            it->applyPowerUp(extraTurn, movementPrecision, attackPrecision, attackPower);
            powerUps.erase(it);  // Eliminar el Power-Up del tablero
            return true;
        }
    }
    return false;
}