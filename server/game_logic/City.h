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

class City {
private:
    CityName name;
    std::vector<StaticObjectData> staticObjects;
    std::vector<BridgeSensorData> bridgeSensors;

    std::string getYamlFileName() const;

public:
    explicit City(CityName name);
    ~City();

    const std::vector<StaticObjectData> getStaticObjects() const;

    const std::vector<BridgeSensorData>& getBridgeSensors() const;

    CityName getCityName() const { return name; }
};


#endif
