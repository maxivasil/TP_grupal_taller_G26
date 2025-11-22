#ifndef CITY_H
#define CITY_H

#include <string>
#include <vector>

enum CityName { LibertyCity, SanAndreas, ViceCity };

struct StaticObjectData {
    float x;
    float y;
    float width;
    float height;
    bool isUp;
};

struct BridgeSensorData {
    float x;
    float y;
    float width;
    float height;
};

struct NPCSpawnZone {
    float x;
    float y;
    float width;
    float height;
};

struct NPCTrafficConfig {
    bool enabled = false;
    int total_npcs = 120;
    int parked_npcs = 40;
    std::vector<NPCSpawnZone> spawn_points;
};

class City {
private:
    CityName name;
    std::vector<StaticObjectData> staticObjects;
    std::vector<BridgeSensorData> bridgeSensors;
    NPCTrafficConfig npcTrafficConfig;

    std::string getYamlFileName() const;

public:
    explicit City(CityName name);
    ~City();

    const std::vector<StaticObjectData> getStaticObjects() const;

    const std::vector<BridgeSensorData>& getBridgeSensors() const;
    
    const NPCTrafficConfig& getNPCTrafficConfig() const { return npcTrafficConfig; }

    CityName getCityName() const { return name; }
};


#endif