#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "Car.h"
#include "CarUpgrades.h"

class Player {
private:
    std::string name;
    int id;
    std::unique_ptr<Car> car;
    CarStats stats;
    uint8_t car_type;  // 0=Van, 1=Ferrari, 2=Celeste, 3=Jeep, 4=Pickup, 5=Limo, 6=Descapotable

public:
    Player(std::string name, int id, CarStats& stats, uint8_t car_type = 0);
    ~Player();

    void initCar(b2WorldId world, b2Vec2 startPos, b2Rot rot);

    void accelerate();

    void brake();

    void turn(Direction direction);

    void activateInfiniteHealthCheat();

    b2Vec2 getPosition() const;

    float getCurrentHealth() const;

    Car* getCar() const;

    const CarStats getCarStats() const;

    int getId() const;

    const std::string& getName() const;

    b2Rot getRotation() const;

    bool isOnBridge() const;

    // Get the magnitude of the car's velocity (speed)
    float getSpeed() const;

    /**
     * @brief Aplica mejoras acumuladas al auto del jugador.
     * @param upgrades Las mejoras a aplicar (penalización de tiempo incluida en lógica superior)
     */
    void applyCarUpgrades(const CarUpgrades& upgrades);

    /**
     * @brief Obtiene las mejoras actualmente aplicadas al auto del jugador.
     * @return Referencia constante a las mejoras del auto
     */
    const CarUpgrades& getCarUpgrades() const;

    /**
     * @brief Obtiene el tipo de auto del jugador.
     * @return car_type (0-6)
     */
    uint8_t getCarType() const;

    /**
     * @brief Verifica si el auto tiene vida infinita activada.
     * @return true si tiene vida infinita, false en caso contrario
     */
    bool hasInfiniteHealth() const;
};

#endif
