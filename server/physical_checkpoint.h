#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <box2d/box2d.h>


struct PhysicalCheckpoint {
    int id;
    b2BodyId body;
    b2Vec2 position;
};


#endif
