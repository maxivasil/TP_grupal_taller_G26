#ifndef RACE_H
#define RACE_H

#include <chrono>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../common/queue.h"
#include "../cmd/cmd_base_server.h"
#include "../cmd/server_to_client_snapshot.h"

#include "BridgeSensorManager.h"
#include "CheckpointManager.h"
#include "City.h"
#include "PhysicsEngine.h"
#include "Player.h"
#include "StaticObject.h"
#include "Track.h"

#define MAX_RACE_TIME 600.0f

// enum CommandType { ACCELERATE, BRAKE, TURN_LEFT, TURN_RIGHT };

// struct Command {
//     int playerId;
//     CommandType type;
// };

class Race {
private:
    City city;
    Track track;
    CheckpointManager checkpointManager;
    BridgeSensorManager bridgeSensorManager;
    std::list<StaticObject> staticObjects;
    PhysicsEngine physics;
    std::vector<std::unique_ptr<Player>>& players;

    // std::chrono::steady_clock::time_point startTime;
    // std::unordered_map<int, float> finishTimes;
    bool finished;

    void checkFinishConditions();
    void initCheckpoints(b2WorldId world);
    void initStaticObjects(b2WorldId world);
    void initBridgeSensors(b2WorldId world);
    void initCars(b2WorldId world);

public:
    Race(CityName cityName, std::string& trackFile, std::vector<std::unique_ptr<Player>>& players);
    ~Race();

    // void applyCommand(const Command& cmd);

    void updatePhysics(float dt);

    bool isFinished() const;

    void turnPlayer(int playerId, Direction dir);
    void acceleratePlayer(int playerId);
    void brakePlayer(int playerId);
    std::vector<CarSnapshot> getSnapshot() const;
    // std::unordered_map<int, float> getFinishTimes() const { return finishTimes; }
};


#endif
