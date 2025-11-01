#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "car.h"

class Player {
private:
    std::string name;
    int id;
    std::unique_ptr<Car> car;
    CarStats stats;

public:
    Player(std::string name, int id, CarStats& stats);
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
};


#endif
