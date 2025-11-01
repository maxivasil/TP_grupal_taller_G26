#ifndef CITY_H
#define CITY_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>

enum CityName {
    LibertyCity,
    SanAndreas,
    ViceCity
};

struct StaticObjectData {
    float x, y, width, height;
    bool isUp;  // true = CollisionUp, false = CollisionDown
};

struct BridgeSensorData {
    float x, y, width, height;
};

class City {
private:
    CityName name;
    std::vector<StaticObjectData> staticObjects;
    std::vector<BridgeSensorData> bridgeSensors;
    
    std::string getYamlFileName() const;

public:
    City(CityName name);
    ~City();
    
    const std::vector<StaticObjectData>& getStaticObjects() const;
    const std::vector<BridgeSensorData>& getBridgeSensors() const;
    
    void render(SDL_Renderer* renderer, const SDL_FRect& camera) const;
};

#endif
