#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>

struct CheckpointData {
    float x;
    float y;
    float width;
    float height;
};

struct InitialDirection {
    float x;
    float y;
};

class Track {
private:
    std::vector<CheckpointData> checkpoints;
    InitialDirection initialDirection;

public:
    explicit Track(std::string& filename);
    ~Track();

    const std::vector<CheckpointData> getCheckpoints() const;

    const InitialDirection getInitialDirection() const;
};


#endif
