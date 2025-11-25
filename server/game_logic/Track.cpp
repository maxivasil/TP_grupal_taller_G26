#include "Track.h"

#include <yaml-cpp/yaml.h>

#include "../../common/constants.h"

Track::Track(const std::string& filename) {
    YAML::Node config = YAML::LoadFile(ABS_DIR "tracks/" + filename);

    if (config["initial_direction"]) {
        YAML::Node dir = config["initial_direction"];
        initialDirection.x = dir["x"].as<float>();
        initialDirection.y = dir["y"].as<float>();
    } else {
        initialDirection.x = 1.0f;
        initialDirection.y = 0.0f;
    }

    if (config["checkpoints"]) {
        for (const auto& node: config["checkpoints"]) {
            CheckpointData cp;
            cp.x = node["x"].as<float>();
            cp.y = node["y"].as<float>();
            cp.width = node["width"].as<float>();
            cp.height = node["height"].as<float>();
            checkpoints.push_back(cp);
        }
    }
}

const std::vector<CheckpointData> Track::getCheckpoints() const { return checkpoints; }

const InitialDirection Track::getInitialDirection() const { return initialDirection; }

Track::~Track() {}
