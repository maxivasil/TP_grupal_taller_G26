#include "NPCTraffic.h"
#include "../../common/CarStats.h"
#include <iostream>
#include <cstdlib>

NPCTraffic::NPCTraffic(b2WorldId world, uint8_t carType_, b2Vec2 position):
        carType(carType_), car(nullptr), currentDirection(0), stuckTimer(0.0f),
        collisionCount(0), isParked(false), lastPosition(position), 
        directionChangeTimer(0.0f), currentMovementDir(Direction::FORWARD) {
    // Obtener stats válidos según el tipo de auto
    std::vector<std::string> carNames = CarStatsDatabase::getAllCarNames();
    std::string selectedCar = carNames[carType_ % carNames.size()];
    CarStats stats = CarStatsDatabase::getCarStats(selectedCar);
    
    // Spawnear mirando hacia el Norte (-Y) para mejor posicionamiento inicial
    car = new NPCCar(world, stats, position, b2MakeRot(-1.5708f), false, carType_);  // -π/2 radianes
}

NPCTraffic::~NPCTraffic() {
    if (car) delete car;
}

void NPCTraffic::updatePhysics(float deltaTime, const void* route) {
    if (!car || car->isDestroyed()) {
        return;
    }
    
    // Los NPCs simplemente avanzan siguiendo su dirección actual
    // Las intersecciones son manejadas por WorldContactListener -> chooseIntersectionDirection
    CarInput input = {true, false, Direction::FORWARD};  // accelerating=true, braking=false
    car->updatePhysics(input);
}

b2Vec2 NPCTraffic::getPosition() const {
    return car ? car->getPosition() : b2Vec2{0, 0};
}

b2Rot NPCTraffic::getRotation() const {
    return car ? car->getRotation() : b2Rot();
}

b2Vec2 NPCTraffic::getLinearVelocity() const {
    return car ? car->getLinearVelocity() : b2Vec2{0, 0};
}

void NPCTraffic::onCollision(Collidable* other, float approachSpeed, float deltaTime,
                             const b2Vec2& contactNormal) {
    if (car) {
        car->onCollision(other, approachSpeed, deltaTime, contactNormal);
    }
}

b2Rot NPCTraffic::getRotation(const b2Vec2& contactNormal) const {
    return getRotation();
}

void NPCTraffic::applyCollision(const CollisionInfo& info) {
    if (car) car->applyCollision(info);
}

void NPCTraffic::resetPosition(b2Vec2 newPos) {
    if (car) {
        car->repair();
    }
    stuckTimer = 0.0f;
    collisionCount = 0;
    currentDirection = std::rand() % 4;
}
