#ifndef CAR_H
#define CAR_H

#include <box2d/box2d.h>

#include "../../common/CarStats.h"

#include "CarUpgrades.h"
#include "Collidable.h"

enum class Direction { FORWARD, LEFT, RIGHT };

struct CarInput {
    bool accelerating;
    bool braking;
    Direction turn_direction;
};

class Car: public Collidable {
private:
    b2BodyId body;
    CarStats stats;
    CarUpgrades upgrades;  // Acumulación de mejoras aplicadas
    float current_health;
    bool hasInfiniteHealth;
    bool isOnBridge;
    bool reverseMode;

    b2BodyDef initCarBodyDef(b2Vec2 position, b2Rot rotation);

    void setShape(b2BodyId body);

    void handleAccelerating(bool accelerating, float speed, b2Vec2 forward);

    void handleBraking(bool braking, b2Vec2 velocity);

    void handleTurning(Direction turn_direction, float speed);

    void verifyMaxSpeed(b2Vec2 velocity, float speed);

    float getImpactForce(const Collidable* other, float approachSpeed, float deltaTime);

    float getImpactAngle(const Collidable* other, const b2Vec2& contactNormal);

public:
    Car(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation);
    ~Car();

    void repair();

    void updatePhysics(const CarInput& input);

    void applyCollision(const CollisionInfo& info) override;

    bool isDestroyed() const;

    b2Vec2 getPosition() const;

    b2Vec2 getLinearVelocity() const;

    b2Rot getRotation() const;

    float getCurrentHealth() const;

    float getMass() const override;

    void onCollision(Collidable* other, float approachSpeed, float deltaTime,
                     const b2Vec2& contactNormal) override;

    b2Rot getRotation(const b2Vec2& contactNormal) const override;

    void setInfiniteHealth();

    void setDestroyed();

    void setLevel(bool onBridge);

    bool getIsOnBridge() const;

    /**
     * @brief Aplica las mejoras acumuladas del auto modificando sus estadísticas.
     * @param newUpgrades Las nuevas mejoras a aplicar
     */
    void applyUpgrades(const CarUpgrades& newUpgrades);

    /**
     * @brief Obtiene las mejoras acumuladas del auto.
     * @return Referencia constante a las mejoras aplicadas
     */
    const CarUpgrades& getUpgrades() const;

    /**
     * @brief Obtiene la estadística de velocidad máxima incluyendo upgrades.
     * @return Velocidad máxima actual del auto (stats base + upgrades)
     */
    float getMaxSpeed() const;

    /**
     * @brief Obtiene la estadística de aceleración incluyendo upgrades.
     * @return Aceleración actual del auto (stats base + upgrades)
     */
    float getAcceleration() const;

    /**
     * @brief Obtiene la estadística de controlabilidad incluyendo upgrades.
     * @return Controlabilidad actual del auto (stats base + upgrades)
     */
    float getHandling() const;

    /**
     * @brief Obtiene la salud máxima incluyendo upgrades.
     * @return Salud máxima actual del auto (stats base + upgrades)
     */
    float getMaxHealth() const;
};


#endif
