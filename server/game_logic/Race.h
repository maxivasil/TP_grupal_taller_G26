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
#include "NPCTraffic.h"
#include "PhysicsEngine.h"
#include "Player.h"
#include "StaticObject.h"
#include "Track.h"
#include "npc_route_manager.h"

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
    std::list<StaticObject> staticObjects;
    std::vector<std::unique_ptr<NPCTraffic>> npcs;  // NPCs con física
    NPCRouteManager routeManager;                   // Gestor de rutas para NPCs
    std::vector<NPCRoute> currentRoutes;            // Rutas para la ciudad actual
    PhysicsEngine physics;
    std::vector<std::unique_ptr<Player>>& players;

    std::chrono::steady_clock::time_point startTime;
    std::unordered_map<int, float> playerFinishTimes;
    bool finished;

    void checkFinishConditions();
    void initCheckpoints(b2WorldId world);
    void initStaticObjects(b2WorldId world);
    void initBridgeSensors(b2WorldId world);
    void initCars(b2WorldId world);
    void initNPCs(b2WorldId world);           // Initialize NPC traffic
    void initNPCsFromTrack(b2WorldId world);  // Track-based NPC system

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
