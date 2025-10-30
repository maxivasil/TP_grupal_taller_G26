#ifndef CAR_H
#define CAR_H

#include "collidable.h"

#include <SDL2/SDL.h>
#include <box2d/box2d.h>

class World;

enum class Direction { FORWARD, LEFT, RIGHT, BACKWARD, NONE };

struct CarStats {
    float acceleration;
    float max_speed;
    float turn_speed;
    float mass;
    float brake_force;
    float handling;
    float health_max;
    float length;
    float width;
};

struct CarInput {
    bool accelerating;
    bool braking;
    Direction turn_direction;
};

class Car: public Collidable {
private:
    b2BodyId body;
    CarStats stats;
    float current_health;
    bool hasInfiniteHealth;
    bool isOnBridge;

    b2BodyDef initCarBodyDef(b2Vec2 position, b2Rot rotation);

    void setShape(b2BodyId body);

    void handleAccelerating(bool accelerating, float speed, b2Vec2 forward);

    void handleBraking(bool braking, b2Vec2 velocity);

    void handleTurning(Direction turn_direction, float speed);

    void verifyMaxSpeed(b2Vec2 velocity, float speed);

    float getImpactForce(const Collidable* other, float approachSpeed, float deltaTime);

    float getImpactAngle(const Collidable* other, const b2Vec2& contactNormal);

public:
    Car(b2WorldId world, const CarStats& stats_, b2Vec2 position, b2Rot rotation);
    ~Car();

      /**
     * @brief Construye un nuevo Car en la posición indicada.
     * @param x Posición X inicial en el mundo.
     * @param y Posición Y inicial en el mundo.
     */

    /**
     * @brief Actualiza la posición y estado del auto según la entrada del jugador.
     * @param deltaTime Tiempo transcurrido desde el último frame (en segundos).
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza el auto en pantalla.
     * @param renderer Puntero al renderer de SDL.
     * @param camera Cámara actual (para transformar coordenadas de mundo a pantalla).
     */
    void render(SDL_Renderer* renderer, const SDL_FRect& camera) const;

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

    void setLevel(bool onBridge);

    bool getIsOnBridge() const;

    float getAngle() const;
};


#endif
