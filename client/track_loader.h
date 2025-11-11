#ifndef CLIENT_TRACK_LOADER_H
#define CLIENT_TRACK_LOADER_H

#include "minimap.h"
#include <vector>
#include <string>
#include <filesystem>

class TrackLoader {
public:
    /**
     * Load checkpoints from a YAML track file
     * @param trackName Name of the track file (e.g., "track_four_checkpoints_vice_city")
     * @return Vector of RaceCheckpoint objects
     */
    static std::vector<RaceCheckpoint> loadTrackCheckpoints(const std::string& trackName);
    
    /**
     * Get list of available track files in the tracks directory
     * @return Vector of track file names
     */
    static std::vector<std::string> getAvailableTracks();

private:
    static const std::string TRACKS_DIR;
    static const std::string TRACKS_DIR_ALT;
};

#endif
