#ifndef CLIENT_TRACK_LOADER_H
#define CLIENT_TRACK_LOADER_H

#include "minimap.h"
#include <vector>
#include <string>
#include <filesystem>

class TrackLoader {
public:
   
    static std::vector<RaceCheckpoint> loadTrackCheckpoints(const std::string& trackName);
   
    static std::vector<std::string> getAvailableTracks();

private:
    static const std::string TRACKS_DIR;
    static const std::string TRACKS_DIR_ALT;
};

#endif
