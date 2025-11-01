#include "City.h"

#include <yaml-cpp/yaml.h>

#define LIBERTY_CITY_MAP_PATH "cities/liberty_city.yaml"
#define SAN_ANDREAS_MAP_PATH "cities/san_andreas.yaml"
#define VICE_CITY_MAP_PATH "cities/vice_city.yaml"

City::City(CityName name): name(name) {
    std::string filename = getYamlFileName();
    YAML::Node data = YAML::LoadFile(filename);

    for (const auto& obj: data["objects"]) {
        std::string type = obj["type"].as<std::string>();

        if (type == "CollisionUp" || type == "CollisionDown") {
            staticObjects.push_back({obj["x"].as<float>(), obj["y"].as<float>(),
                                     obj["width"].as<float>(), obj["height"].as<float>(),
                                     (type == "CollisionUp")});
        } else if (type == "BridgeLevelSensor") {
            bridgeSensors.push_back({obj["x"].as<float>(), obj["y"].as<float>(),
                                     obj["width"].as<float>(), obj["height"].as<float>()});
        }
    }
}

std::string City::getYamlFileName() const {
    switch (name) {
        case LibertyCity:
            return LIBERTY_CITY_MAP_PATH;
        case SanAndreas:
            return SAN_ANDREAS_MAP_PATH;
        case ViceCity:
            return VICE_CITY_MAP_PATH;
    }
    return "";
}

const std::vector<StaticObjectData> City::getStaticObjects() const { return staticObjects; }

const std::vector<BridgeSensorData>& City::getBridgeSensors() const { return bridgeSensors; }

City::~City() {}
