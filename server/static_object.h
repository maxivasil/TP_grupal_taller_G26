#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include "collidable.h"

#include <box2d/box2d.h>


struct StaticObjectParam {
    float length;
    float width;
    float mass;
    bool onBridge;
};


class StaticObject: public Collidable {
private:
    b2BodyId body;
    float mass;
    bool onBridge;

    b2BodyDef initStaticObjectBodyDef(b2Vec2 position);

    void setShape(b2BodyId body, float width, float length);

public:
    StaticObject(b2WorldId world, b2Vec2 position, StaticObjectParam& params);

    void onCollision(Collidable* other, float approachSpeed, float deltaTime,
                     const b2Vec2& contactNormal) override;

    virtual float getMass() const override;

    b2Rot getRotation(const b2Vec2& contactNormal) const override;

    void applyCollision(const CollisionInfo& info) override;

    ~StaticObject();
};


#endif
