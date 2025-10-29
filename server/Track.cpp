#include "Track.h"

#include <yaml-cpp/yaml.h>

Track::Track(std::string& filename) {
    YAML::Node config = YAML::LoadFile(filename);

    for (const auto& node: config["checkpoints"]) {
        CheckpointData cp;
        cp.x = node["x"].as<float>();
        cp.y = node["y"].as<float>();
        cp.width = node["width"].as<float>();
        cp.height = node["height"].as<float>();
        checkpoints.push_back(cp);
    }
}

const std::vector<CheckpointData> Track::getCheckpoints() const { return checkpoints; }

Track::~Track() {}
