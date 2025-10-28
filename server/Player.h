#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "Car.h"

class Player {
private:
    std::string name;
    std::unique_ptr<Car> car;

public:
    explicit Player(std::string name);
    ~Player();

    void assignCar(std::unique_ptr<Car> newCar);

    void accelerate();

    void brake();

    void turn(Direction direction);

    void activateInfiniteHealthCheat();

    b2Vec2 getPosition() const;

    float getCurrentHealth() const;

    Car* getCar() const;
};


#endif
