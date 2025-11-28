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

struct IntersectionData {
    float x;
    float y;
    float width;
    float height;
    bool up;
    bool down;
    bool right;
    bool left;
};

class City {
private:
    CityName name;
    std::vector<StaticObjectData> staticObjects;
    std::vector<BridgeSensorData> bridgeSensors;
    std::vector<IntersectionData> intersections;

    std::string getYamlFileName() const;

public:
    explicit City(CityName name);
    ~City();

    const std::vector<StaticObjectData>& getStaticObjects() const;

    const std::vector<BridgeSensorData>& getBridgeSensors() const;

    const std::vector<IntersectionData>& getIntersections() const;

    CityName getCityName() const { return name; }
};


#endif
