#ifndef CLIENT_TRACK_LOADER_H
#define CLIENT_TRACK_LOADER_H

#include <filesystem>
#include <string>
#include <vector>

#include "minimap.h"

class TrackLoader {
public:
    static std::vector<RaceCheckpoint> loadTrackCheckpoints(const std::string& trackName);
};

#endif
