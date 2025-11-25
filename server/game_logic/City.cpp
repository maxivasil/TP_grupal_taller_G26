#include "City.h"

#include <iostream>

#include <yaml-cpp/yaml.h>

#include "../../common/constants.h"

#define LIBERTY_CITY_MAP_PATH "cities/liberty_city.yaml"
#define SAN_ANDREAS_MAP_PATH "cities/san_andreas.yaml"
#define VICE_CITY_MAP_PATH "cities/vice_city.yaml"

City::City(CityName name): name(name) {
    std::string filename = getYamlFileName();
    YAML::Node data = YAML::LoadFile(ABS_DIR + filename);

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

    // Load NPC traffic configuration if it exists
    if (data["npc_traffic"]) {
        const auto& npcConfig = data["npc_traffic"];
        npcTrafficConfig.enabled = npcConfig["enabled"].as<bool>(false);
        npcTrafficConfig.total_npcs = npcConfig["total_npcs"].as<int>(120);
        npcTrafficConfig.parked_npcs = npcConfig["parked_npcs"].as<int>(40);

        // Load spawn points
        if (npcConfig["spawn_points"]) {
            for (const auto& spawn: npcConfig["spawn_points"]) {
                npcTrafficConfig.spawn_points.push_back(
                        {spawn["x"].as<float>(), spawn["y"].as<float>(), spawn["width"].as<float>(),
                         spawn["height"].as<float>()});
            }
        }

        std::cout << "[CITY] NPC Traffic loaded for " << filename << ": "
                  << npcTrafficConfig.total_npcs << " moving, " << npcTrafficConfig.parked_npcs
                  << " parked, " << npcTrafficConfig.spawn_points.size() << " spawn zones"
                  << std::endl;
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
