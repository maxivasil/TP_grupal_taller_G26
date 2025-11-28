#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

enum SensorType { Checkpoint, BridgeLevel, Intersection };

enum DirectionMask {
    DIR_UP = 1 << 0,     // 1
    DIR_DOWN = 1 << 1,   // 2
    DIR_RIGHT = 1 << 2,  // 4
    DIR_LEFT = 1 << 3    // 8
};

struct SensorData {
    SensorType type;
    int id;  // Cuando type = 'Intersection', este campo almacena la máscara de bits (DirectionMask)
};

#endif
