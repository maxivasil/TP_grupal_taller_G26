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
#include "../cmd/server_to_client_snapshot_server.h"

#include "BridgeSensorManager.h"
#include "CheckpointManager.h"
#include "City.h"
#include "IntersectionManager.h"
#include "NPCCar.h"
#include "PhysicsEngine.h"
#include "Player.h"
#include "StaticObject.h"
#include "Track.h"

#define MAX_RACE_TIME 600.0f

struct RaceInfo {
    CityName city;
    std::string trackFile;
    RaceInfo(CityName city, const std::string& trackFile): city(city), trackFile(trackFile) {}
};

class Race {
private:
    City city;
    Track track;
    CheckpointManager checkpointManager;
    BridgeSensorManager bridgeSensorManager;
    IntersectionManager intersectionManager;
    std::list<StaticObject> staticObjects;
    std::vector<std::unique_ptr<NPCCar>> npcs;  // NPCs con física
    PhysicsEngine physics;
    std::vector<std::unique_ptr<Player>>& players;

    std::chrono::steady_clock::time_point startTime;
    std::unordered_map<int, float> playerFinishTimes;
    bool finished;

    void checkFinishConditions();
    void initCheckpoints(b2WorldId world);
    void initStaticObjects(b2WorldId world);
    void initBridgeSensors(b2WorldId world);
    void initIntersections(b2WorldId world);
    void initCars(b2WorldId world);
    void initNPCs(b2WorldId world);  // Initialize NPC traffic

public:
    Race(CityName cityName, std::string& trackFile, std::vector<std::unique_ptr<Player>>& players);
    ~Race();

    void start();

    void updatePhysics(float dt);

    bool isFinished() const;

    void turnPlayer(int playerId, Direction dir);

    void acceleratePlayer(int playerId);

    void brakePlayer(int playerId);

    std::vector<CarSnapshot> getSnapshot() const;

    const std::unordered_map<int, float>& getFinishTimes() const;

    const std::vector<std::unique_ptr<Player>>& getPlayers() const;

    // Get current elapsed time since race start
    float getCurrentElapsedTime() const;

    // Métodos para cheats
    void activateInfiniteHealthCheat(int playerId);
    void forceWinCheat(int playerId);
    void forceLoseCheat(int playerId);
};


#endif
