#include "Player.h"

#include <utility>
#include <cmath>

Player::Player(std::string name, int id, CarStats& stats, uint8_t car_type):
        name(std::move(name)), id(id), car(nullptr), stats(std::move(stats)), car_type(car_type) {}

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

const std::string& Player::getName() const { return name; }

b2Rot Player::getRotation() const { return car ? car->getRotation() : b2Rot_identity; }

bool Player::isOnBridge() const { return car ? car->getIsOnBridge() : false; }

float Player::getSpeed() const {
    if (!car) {
        return 0.0f;
    }
    
    // Get the linear velocity vector from Box2D
    b2Vec2 velocity = car->getLinearVelocity();
    
    // Calculate the magnitude of the velocity vector
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    
    return speed;
}

Player::~Player() {}

void Player::applyCarUpgrades(const CarUpgrades& upgrades) {
    if (car) {
        car->applyUpgrades(upgrades);
    }
}

const CarUpgrades& Player::getCarUpgrades() const {
    if (car) {
        return car->getUpgrades();
    }
    static CarUpgrades empty;
    return empty;
}

uint8_t Player::getCarType() const {
    return car_type;
}
