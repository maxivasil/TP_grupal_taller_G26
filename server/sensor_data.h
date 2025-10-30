#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

enum SensorType { Checkpoint, BridgeLevel };

struct SensorData {
    SensorType type;
    int id;
};

#endif
