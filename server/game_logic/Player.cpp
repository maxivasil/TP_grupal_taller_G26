#include "Player.h"

#include <utility>

Player::Player(std::string name, int id, CarStats& stats):
        name(std::move(name)), id(id), car(nullptr), stats(std::move(stats)) {}

void Player::initCar(b2WorldId world, b2Vec2 startPos, b2Rot rot) {
    car = std::make_unique<Car>(world, stats, startPos, rot);
}

void Player::accelerate() {
    if (!car) {
        return;
    }
    car->updatePhysics(
            {.accelerating = true, .braking = false, .turn_direction = Direction::FORWARD});
}

void Player::brake() {
    if (!car) {
        return;
    }
    car->updatePhysics(
            {.accelerating = false, .braking = true, .turn_direction = Direction::FORWARD});
}

void Player::turn(Direction direction) {
    if (!car) {
        return;
    }
    car->updatePhysics({.accelerating = false, .braking = false, .turn_direction = direction});
}

void Player::activateInfiniteHealthCheat() {
    if (!car) {
        return;
    }
    car->setInfiniteHealth();
}

b2Vec2 Player::getPosition() const { return car ? car->getPosition() : b2Vec2_zero; }

float Player::getCurrentHealth() const { return car ? car->getCurrentHealth() : 0.0f; }

Car* Player::getCar() const { return car.get(); }

const CarStats Player::getCarStats() const { return stats; }

int Player::getId() const { return id; }

Player::~Player() {}
