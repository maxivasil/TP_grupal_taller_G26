#include "Race.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <utility>
#include <vector>

#define VELOCITY_ITERATIONS 4

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

void Race::initCars(b2WorldId world) {
    const auto& checkpoints = track.getCheckpoints();
    if (!checkpoints.empty()) {
        const auto& start = checkpoints.front();

        b2Vec2 basePos{start.x, start.y};

        InitialDirection dir = track.getInitialDirection();
        b2Vec2 direction = {dir.x, dir.y};

        for (size_t i = 0; i < players.size(); ++i) {
            Player* p = players[i].get();
            float carLength = p->getCarStats().length;
            float offset = i * (carLength + 1.5f);

            b2Vec2 pos = basePos - offset * direction;

            if (i % 2 == 1) {
                pos.y += start.height / 4.0f;
            } else {
                pos.y -= start.height / 4.0f;
            }

            p->initCar(world, pos, b2MakeRot(0));
        }
    }
}

Race::Race(CityName cityName, std::string& trackFile,
           std::vector<std::unique_ptr<Player>>& players):
        city(cityName),
        track(trackFile),
        physics(checkpointManager),
        players(players),
        finished(false) {
    b2WorldId world = physics.getWorld();
    initCheckpoints(world);
    initStaticObjects(world);
    initBridgeSensors(world);
    initCars(world);
}

void Race::updatePhysics(float dt) { physics.step(dt, VELOCITY_ITERATIONS); }

bool Race::isFinished() const { return finished; }

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

    for (const auto& player: players) {
        b2Vec2 pos = player->getPosition();
        snapshot.push_back(CarSnapshot{(uint8_t)player->getId(), pos.x, pos.y, false,
                                       player->getCurrentHealth(), 0, 0});
    }

    return snapshot;
}

Race::~Race() {}
