#include "Race.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#define VELOCITY_ITERATIONS 4
#define MIN_NPCS 50
#define MAX_NPCS 100
#define NPC_SPEED 15.0f

void Race::initCheckpoints(b2WorldId world) {
    for (auto& ckpt: track.getCheckpoints()) {
        checkpointManager.createCheckpoint(world, {ckpt.x, ckpt.y}, ckpt.width, ckpt.height);
    }
}

void Race::initStaticObjects(b2WorldId world) {
    for (auto& obj: city.getStaticObjects()) {
        StaticObjectParam params = {
                .length = obj.height, .width = obj.width, .mass = 10000.0f, .onBridge = obj.isUp};
        staticObjects.emplace_back(world, b2Vec2{obj.x, obj.y}, params);
    }
}

void Race::initBridgeSensors(b2WorldId world) {
    for (auto bridgeSensor: city.getBridgeSensors()) {
        bridgeSensorManager.createBridgeSensor(world, b2Vec2{bridgeSensor.x, bridgeSensor.y},
                                               bridgeSensor.width, bridgeSensor.height);
    }
}

void Race::initIntersections(b2WorldId world) {
    for (auto intersection: city.getIntersections()) {
        intersectionManager.createIntersection(world, b2Vec2{intersection.x, intersection.y},
                                               intersection.width, intersection.height,
                                               intersection.up, intersection.down,
                                               intersection.right, intersection.left);
    }
}

void Race::initCars(b2WorldId world) {
    const auto& checkpoints = track.getCheckpoints();
    if (!checkpoints.empty()) {
        const auto& startCheckpoint = checkpoints.front();

        b2Vec2 basePos{startCheckpoint.x, startCheckpoint.y};

        InitialDirection dir = track.getInitialDirection();
        b2Vec2 direction = {dir.x, dir.y};

        for (size_t i = 0; i < players.size(); ++i) {
            Player* p = players[i].get();
            float carLength = p->getCarStats().length;
            float offset = i * (carLength + 1.5f);

            b2Vec2 pos = basePos - offset * direction;

            if (i % 2 == 1) {
                pos.y += startCheckpoint.height / 4.0f;
            } else {
                pos.y -= startCheckpoint.height / 4.0f;
            }

            p->initCar(world, pos, b2MakeRot(0));
        }
    }
}

void Race::initNPCs(b2WorldId world) {
    std::vector<IntersectionData> spawnPoints = city.getIntersections();
    if (spawnPoints.empty())
        return;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(spawnPoints.begin(), spawnPoints.end(), g);

    int minNPCs = std::max(1, (int)(spawnPoints.size() * 0.4f));
    int maxNPCs = std::max(minNPCs, (int)(spawnPoints.size() * 0.6f));
    int numNPCsToCreate = minNPCs + (std::rand() % (maxNPCs - minNPCs + 1));

    int createdCount = 0;
    std::vector<std::string> carNames = CarStatsDatabase::getAllCarNames();
    float margin = 0.5f;

    for (const auto& inter: spawnPoints) {
        if (createdCount >= numNPCsToCreate)
            break;

        std::vector<float> validAngles;

        if (inter.right)
            validAngles.push_back(0.0f);
        if (inter.up)
            validAngles.push_back(-B2_PI / 2.0f);
        if (inter.left)
            validAngles.push_back(B2_PI);
        if (inter.down)
            validAngles.push_back(B2_PI / 2.0f);

        if (validAngles.empty())
            continue;

        float spawnAngle = validAngles[std::rand() % validAngles.size()];
        std::string randomCarName = carNames[std::rand() % carNames.size()];
        CarStats npcStats = CarStatsDatabase::getCarStats(randomCarName);
        uint8_t carType = CarStatsDatabase::getCarTypeFromName(randomCarName);

        float offsetDist = npcStats.length * 0.5f + margin;
        b2Vec2 spawnPos = b2Vec2{inter.x, inter.y} +
                          b2MulSV(offsetDist, b2RotateVector(b2MakeRot(spawnAngle), {1, 0}));

        npcs.push_back(std::make_unique<NPCCar>(world, npcStats, spawnPos, b2MakeRot(spawnAngle),
                                                false,  // isParked
                                                carType));

        createdCount++;
    }
}

void Race::initParkedCars(b2WorldId world) {
    std::vector<ParkedCarData> parkedAreas = city.getParkedCars();
    if (parkedAreas.empty()) {
        return;
    }

    std::vector<std::string> carNames = CarStatsDatabase::getAllCarNames();

    int minCars = std::max(1, static_cast<int>(parkedAreas.size() * 0.4f));
    int maxCars = std::max(minCars, static_cast<int>(parkedAreas.size() * 0.6f));
    int totalCarsToSpawn = minCars + (std::rand() % (maxCars - minCars + 1));

    int parkedCount = 0;
    const float margin = 0.1f;

    size_t areaIndex = 0;
    while (parkedCount < totalCarsToSpawn) {
        const ParkedCarData& area = parkedAreas[areaIndex % parkedAreas.size()];

        std::string randomCarName = carNames[std::rand() % carNames.size()];
        CarStats carStats = CarStatsDatabase::getCarStats(randomCarName);
        uint8_t carType = CarStatsDatabase::getCarTypeFromName(randomCarName);

        float carWidth = carStats.width;
        float carHeight = carStats.length;
        float widthUsed = area.isVertical ? carWidth : carHeight;
        float heightUsed = area.isVertical ? carHeight : carWidth;

        float availableWidth = area.width - 2 * margin;
        float availableHeight = area.height - 2 * margin;

        float posX = area.x + margin +
                     static_cast<float>(std::rand()) / RAND_MAX * (availableWidth - widthUsed);
        float posY = area.y + margin +
                     static_cast<float>(std::rand()) / RAND_MAX * (availableHeight - heightUsed);

        b2Vec2 position = {posX + widthUsed / 2.0f, posY + heightUsed / 2.0f};

        float angle = 0.0f;
        if (area.isVertical) {
            angle = (std::rand() % 2 == 0) ? B2_PI / 2.0f : -B2_PI / 2.0f;
        } else {
            angle = (std::rand() % 2 == 0) ? 0.0f : B2_PI;
        }
        npcs.push_back(std::make_unique<NPCCar>(world, carStats, position, b2MakeRot(angle), true,
                                                carType));

        parkedCount++;
        areaIndex++;
    }
}

Race::Race(CityName cityName, std::string& trackFile,
           std::vector<std::unique_ptr<Player>>& players):
        city(cityName),
        track(trackFile),
        physics(checkpointManager),
        players(players),
        finished(false),
        isRunning(false) {
    b2WorldId world = physics.getWorld();
    initCheckpoints(world);
    initStaticObjects(world);
    initBridgeSensors(world);
    initIntersections(world);
    initCars(world);
    initNPCs(world);
    initParkedCars(world);
}

void Race::start(std::chrono::steady_clock::time_point offsetTime) {
    raceTimeOffset = offsetTime;
    isRunning = true;
    finished = false;
}

void Race::checkFinishConditions() {
    for (const auto& player: players) {
        if (checkpointManager.hasCarFinished(player->getCar()) &&
            !playerFinishTimes.count(player->getId())) {

            float elapsed = getCurrentElapsedTime();
            playerFinishTimes[player->getId()] = elapsed;
        }
        if (player->getCurrentHealth() <= 0.0f && !playerFinishTimes.count(player->getId())) {
            playerFinishTimes[player->getId()] = -1.0f;
        }
    }

    if (playerFinishTimes.size() == players.size() || getCurrentElapsedTime() >= MAX_RACE_TIME) {
        finished = true;
    }
}

void Race::updatePhysics(float dt) {
    // Skip physics update if paused (during countdown)
    if (isPhysicsPaused) {
        return;
    }

    for (auto& npc: npcs) {
        if (!npc->isDestroyed()) {
            npc->updatePhysics({});
        }
    }

    physics.step(dt, VELOCITY_ITERATIONS);
    checkFinishConditions();
}

bool Race::isFinished() const { return finished; }

float Race::getCurrentElapsedTime() const {
    if (!isRunning) {
        return 0.0f;
    }
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(now - raceTimeOffset).count();
}

void Race::turnPlayer(int playerId, Direction dir) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        (*it)->turn(dir);
    }
}

void Race::acceleratePlayer(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        (*it)->accelerate();
    }
}

void Race::brakePlayer(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        (*it)->brake();
    }
}

std::vector<CarSnapshot> Race::getSnapshot() const {
    std::vector<CarSnapshot> snapshot;

    // Agregar snapshots de jugadores
    for (const auto& player: players) {
        b2Vec2 pos = player->getPosition();
        b2Rot rot = player->getRotation();
        float angle = b2Rot_GetAngle(rot) * 180.0f / B2_PI;
        float speed = player->getSpeed();
        CarSnapshot cs{player->getId(),
                       pos.x,
                       pos.y,
                       false,
                       player->getCurrentHealth(),
                       speed,
                       angle,
                       player->isOnBridge(),
                       player->getCarType()};
        cs.hasInfiniteHealth = player->hasInfiniteHealth();
        cs.isNPC = false;
        cs.playerName = player->getName();  // Agregar nombre del jugador
        snapshot.push_back(cs);
    }

    // Agregar snapshots de NPCs
    // IDs de NPCs comienzan en 100 para evitar conflictos con IDs de jugadores (típicamente 0-3)
    uint8_t npcIdOffset = 100;
    for (size_t i = 0; i < npcs.size(); ++i) {
        const auto& npc = npcs[i];
        b2Vec2 pos = npc->getPosition();
        b2Rot rot = npc->getRotation();
        float angle = b2Rot_GetAngle(rot) * 180.0f / B2_PI;
        b2Vec2 velocity = npc->getLinearVelocity();
        float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

        // Obtener el estado de puente del NPC
        bool onBridge = npc->getIsOnBridge();

        CarSnapshot cs{(uint8_t)(npcIdOffset + i), pos.x, pos.y, false,
                       npc->getCurrentHealth(),    speed, angle, onBridge,
                       npc->getCarType()};
        cs.isNPC = true;
        snapshot.push_back(cs);
    }

    return snapshot;
}


const std::unordered_map<int, float>& Race::getFinishTimes() const { return playerFinishTimes; }

const std::vector<std::unique_ptr<Player>>& Race::getPlayers() const { return players; }

Race::~Race() {}

// Métodos para cheats
void Race::activateInfiniteHealthCheat(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        (*it)->getCar()->setInfiniteHealth();
    }
}

void Race::forceWinCheat(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        // Marcar como finalizador inmediatamente
        float elapsed = getCurrentElapsedTime();
        playerFinishTimes[playerId] = elapsed;
    }
}

void Race::forceLoseCheat(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        (*it)->getCar()->setDestroyed();
    }
}
