#ifndef COLLISION_CATEGORIES_H
#define COLLISION_CATEGORIES_H

enum CollisionCategories {
    CategoryStatic = 0x1,
    CategoryDynamic = 0x2,
    CategorySensor = 0x4,
    CategoryAll = 0xFFFFFFFFFFFFFFFF,
};

#endif
