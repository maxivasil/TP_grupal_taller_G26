#ifndef COLLIDABLE_H
#define COLLIDABLE_H

<<<<<<<< HEAD:game/collidable.h
#include "collision_info.h"

========
>>>>>>>> origin/lógica_de_juego:server/game_logic/Collidable.h
#include <box2d/box2d.h>

#include "CollisionInfo.h"

class Collidable {
public:
    virtual ~Collidable() = default;

    virtual void onCollision(Collidable* other, float approachSpeed, float deltaTime,
                             const b2Vec2& contactNormal) = 0;

    virtual float getMass() const = 0;

    virtual b2Rot getRotation(const b2Vec2& contactNormal) const = 0;

    virtual void applyCollision(const CollisionInfo& info) = 0;
};

#endif
