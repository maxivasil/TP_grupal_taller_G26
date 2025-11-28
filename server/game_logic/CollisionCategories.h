#ifndef COLLISION_CATEGORIES_H
#define COLLISION_CATEGORIES_H

enum CollisionCategories {
    CategoryGroundCar = 0x0001,
    CategoryBridgeCar = 0x0002,
    CategoryGroundObj = 0x0004,
    CategoryBridgeObj = 0x0008,
    CategoryBridgeSensor = 0x0010,
    CategoryCheckpoint = 0x0020,
    CategoryIntersection = 0x0040,
    CategoryAll = 0xFFFF
};

#endif
