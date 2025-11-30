#include "track_loader.h"

#include <filesystem>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include "../../common/constants.h"


std::vector<RaceCheckpoint> TrackLoader::loadTrackCheckpoints(const std::string& trackFile) {
    std::vector<RaceCheckpoint> checkpoints;

    try {
        YAML::Node config = YAML::LoadFile(ABS_DIR "tracks/" + trackFile);

        if (!config["checkpoints"]) {
            std::cerr << "ERROR: No 'checkpoints' field in track file" << std::endl;
            return checkpoints;
        }

        const YAML::Node& checkpointsNode = config["checkpoints"];

        if (!checkpointsNode.IsSequence()) {
            std::cerr << "ERROR: 'checkpoints' is not a sequence" << std::endl;
            return checkpoints;
        }

        int id = 0;

        for (const auto& cpNode: checkpointsNode) {
            bool isFinish = false;
            RaceCheckpoint cp;
            cp.id = id;
            cp.x = cpNode["x"].as<float>();
            cp.y = cpNode["y"].as<float>();
            cp.width = cpNode["width"].as<float>();
            cp.height = cpNode["height"].as<float>();

            isFinish = (id == (int)checkpointsNode.size() - 1);
            cp.isFinish = isFinish;

            checkpoints.push_back(cp);

            id++;
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "ERROR: Failed to parse track file: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    return checkpoints;
}
