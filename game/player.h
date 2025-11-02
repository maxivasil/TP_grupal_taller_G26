#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <set>

class Player {
private:
    int totalCheckpoints;
    int checkpointsCompleted;
    std::set<int> completedCheckpointIds;  // Para evitar duplicados
    float startTime;
    float finishTime;
    bool raceFinished;
    
public:
    Player();
    
    void setTotalCheckpoints(int total) { totalCheckpoints = total; }
    void completeCheckpoint(int checkpointId);
    void finishRace();
    void reset();
    
    int getCheckpointsCompleted() const { return checkpointsCompleted; }
    int getTotalCheckpoints() const { return totalCheckpoints; }
    float getElapsedTime() const;
    float getFinishTime() const { return finishTime; }
    bool hasFinished() const { return raceFinished; }
    bool hasCompletedCheckpoint(int checkpointId) const;
    float getProgress() const;  // Retorna porcentaje (0.0 - 1.0)
};

#endif
