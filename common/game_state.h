#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include <cstdint>
#include <box2d/box2d.h>

class b2World;

/**
 * @brief Representa el estado actual del juego, usado para sincronizar con el cliente.
 */
struct CarState {
    uint32_t id;
    float x;
    float y;
    float angle;
    float speed;
    float health;
};

/**
 * @brief Estado global del juego (autos, NPCs, tiempo, etc.).
 */
class GameState {
public:
    /**
     * @brief Actualiza el estado de los autos según el mundo físico.
     * @param world Mundo Box2D actual.
     */
    void update_from_world(b2WorldId worldId);

    /**
     * @brief Devuelve los autos actuales.
     */
    const std::vector<CarState>& get_cars() const;

    /**
     * @brief Limpia el estado actual.
     */
    void clear();

private:
    std::vector<CarState> cars_;
};

#endif  // GAME_STATE_H
