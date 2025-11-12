#include "track_loader.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <filesystem>

const std::string TrackLoader::TRACKS_DIR = "tracks";
const std::string TrackLoader::TRACKS_DIR_ALT = "../tracks";

std::vector<RaceCheckpoint> TrackLoader::loadTrackCheckpoints(const std::string& trackName) {
    std::vector<RaceCheckpoint> checkpoints;
    
    // Try to find the track file
    std::string trackPath;
    if (std::filesystem::exists(TRACKS_DIR + "/" + trackName + ".yaml")) {
        trackPath = TRACKS_DIR + "/" + trackName + ".yaml";
    } else if (std::filesystem::exists(TRACKS_DIR_ALT + "/" + trackName + ".yaml")) {
        trackPath = TRACKS_DIR_ALT + "/" + trackName + ".yaml";
    } else {
        std::cerr << "ERROR: Track file not found: " << trackName << ".yaml" << std::endl;
        std::cerr << "Searched in: " << TRACKS_DIR << " and " << TRACKS_DIR_ALT << std::endl;
        return checkpoints;
    }
    
    try {
        std::cout << "Loading track from: " << trackPath << std::endl;
        YAML::Node config = YAML::LoadFile(trackPath);
        
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
        bool isFinish = false;
        
        for (const auto& cpNode : checkpointsNode) {
            RaceCheckpoint cp;
            cp.id = id;
            cp.x = cpNode["x"].as<float>();
            cp.y = cpNode["y"].as<float>();
            cp.width = cpNode["width"].as<float>();
            cp.height = cpNode["height"].as<float>();
            
            // Last checkpoint is the finish line
            isFinish = (id == (int)checkpointsNode.size() - 1);
            cp.isFinish = isFinish;
            
            checkpoints.push_back(cp);
            
            std::cout << "  Checkpoint " << id << ": (" << cp.x << ", " << cp.y << ") "
                      << (isFinish ? "[FINISH]" : "[CHECKPOINT]") << std::endl;
            
            id++;
        }
        
        std::cout << "Successfully loaded " << checkpoints.size() << " checkpoints" << std::endl;
        
    } catch (const YAML::Exception& e) {
        std::cerr << "ERROR: Failed to parse track file: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    
    return checkpoints;
}

std::vector<std::string> TrackLoader::getAvailableTracks() {
    std::vector<std::string> tracks;
    
    std::string tracksDir = TRACKS_DIR;
    if (!std::filesystem::exists(tracksDir)) {
        tracksDir = TRACKS_DIR_ALT;
    }
    
    if (!std::filesystem::exists(tracksDir)) {
        std::cerr << "ERROR: Tracks directory not found" << std::endl;
        return tracks;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(tracksDir)) {
            if (entry.path().extension() == ".yaml") {
                tracks.push_back(entry.path().stem().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    
    return tracks;
}
