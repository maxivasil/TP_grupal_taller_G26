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


class Track {
private:
    std::vector<CheckpointData> checkpoints;

public:
    explicit Track(std::string& filename);
    ~Track();

    const std::vector<CheckpointData> getCheckpoints() const;
};


#endif
