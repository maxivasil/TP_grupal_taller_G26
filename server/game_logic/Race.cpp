#include "Race.h"
#include "npc_route_manager.h"
#include "npc_checkpoint_loader.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <cstdlib>
#include <cmath>

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
    // Determinar nombre de ciudad
    std::string cityName;
    CityName cn = city.getCityName();
    
    std::cout << "[RACE] City enum value: " << (int)cn << std::endl;
    
    if (cn == LibertyCity) {
        cityName = "liberty_city";
        std::cout << "[RACE] Detected: LibertyCity (enum=0)" << std::endl;
    } else if (cn == SanAndreas) {
        cityName = "san_andreas";
        std::cout << "[RACE] Detected: SanAndreas (enum=1)" << std::endl;
    } else if (cn == ViceCity) {
        cityName = "vice_city";
        std::cout << "[RACE] Detected: ViceCity (enum=2)" << std::endl;
    } else {
        cityName = "san_andreas";
        std::cout << "[RACE] Unknown city, defaulting to san_andreas" << std::endl;
    }
    
    std::cout << "[RACE] Initializing NPCs for city: " << cityName << std::endl;
    
    // Cargar checkpoints desde el archivo YAML
    std::string yamlPath = NPCCheckpointLoader::getCheckpointFilePath(cityName);
    std::vector<SimpleCheckpoint> simpleCheckpoints = NPCCheckpointLoader::loadCheckpointsFromYAML(yamlPath);
    
    if (simpleCheckpoints.empty()) {
        std::cerr << "[RACE ERROR] Failed to load checkpoints from " << yamlPath << std::endl;
        return;
    }
    
    // Convertir SimpleCheckpoint a NPCCheckpoint
    std::vector<NPCCheckpoint> checkpoints;
    for (const auto& scp : simpleCheckpoints) {
        checkpoints.push_back(NPCCheckpoint{scp.x, scp.y, scp.width, scp.height});
    }
    
    // Generar rutas desde los checkpoints del YAML
    routeManager.initializeCityFromCheckpoints(cityName, checkpoints);
    
    const auto& routes = routeManager.getRoutes(cityName);
    currentRoutes.assign(routes.begin(), routes.end());
    
    std::cout << "[RACE] Generated " << currentRoutes.size() << " routes for city: " << cityName << std::endl;
    
    // Distribuir 6 NPCs por ruta (60 total para 10 rutas)
    // Cada NPC inicia en un punto diferente y separado en la ruta
    int npcCount = 0;
    int npcsPerRoute = 6;
    int maxNPCs = 60;
    
    for (size_t routeIdx = 0; routeIdx < currentRoutes.size(); ++routeIdx) {
        const auto& route = currentRoutes[routeIdx];
        if (route.points.empty()) continue;
        
        // Distribuir NPCs equitativamente a lo largo de la ruta
        size_t routeLength = route.points.size();
        size_t spacing = (routeLength > npcsPerRoute) ? (routeLength / npcsPerRoute) : 1;
        
        for (int i = 0; i < npcsPerRoute && npcCount < maxNPCs; ++i) {
            // Calcular índice base espaciado uniformemente
            size_t basePointIdx = (i * spacing) % routeLength;
            
            // Agregar variación aleatoria pequeña para no estar exactamente en línea
            int randomOffset = (rand() % 5) - 2;  // ±2 puntos
            size_t pointIdx = (basePointIdx + randomOffset + routeLength) % routeLength;
            
            const auto& point = route.points[pointIdx];
            
            // Agregar pequeño offset perpendicular para no superponer
            float offsetX = (rand() % 10 - 5) * 0.1f;  // ±0.5
            float offsetY = (rand() % 10 - 5) * 0.1f;
            
            b2Vec2 pos{point.x + offsetX, point.y + offsetY};
            uint8_t carType = rand() % 7;  // 7 tipos de autos (0-6)
            
            auto npc = std::make_unique<NPCTraffic>(world, carType, pos);
            npc->setRoute(&route.points);  // Asignar la ruta al NPC
            npcs.push_back(std::move(npc));
            
            std::cout << "[RACE] NPC #" << npcCount << " created at (" << pos.x << ", " << pos.y 
                      << ") on route " << (routeIdx + 1) << " with " << route.points.size() 
                      << " waypoints [Car Type: " << (int)carType << "]" << std::endl;
            
            npcCount++;
        }
    }
    
    std::cout << "[RACE] Initialized " << npcCount << " NPC traffic vehicles across " 
              << currentRoutes.size() << " routes" << std::endl;
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
    initNPCs(world);
}

void Race::start() {
    startTime = std::chrono::steady_clock::now();
    finished = false;
}

void Race::checkFinishConditions() {
    for (const auto& player: players) {
        if (checkpointManager.hasCarFinished(player->getCar()) &&
            !playerFinishTimes.count(player->getId())) {

            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - startTime).count();
            playerFinishTimes[player->getId()] = elapsed;

            std::cout << "[RACE] Player " << player->getId() << " (" << player->getName()
                      << ") finished in " << elapsed << "s" << std::endl;
        }
        if (player->getCurrentHealth() <= 0.0f && !playerFinishTimes.count(player->getId())) {
            playerFinishTimes[player->getId()] = -1.0f;

            std::cout << "[RACE] Player " << player->getId() << " (" << player->getName()
                      << ") destroyed their car" << std::endl;
        }
    }

    // std::cout << "[RACE] Finish check: " << playerFinishTimes.size() << "/" << players.size()
    //           << " players finished" << std::endl;

    if (playerFinishTimes.size() == players.size() ||
        std::chrono::duration<float>(std::chrono::steady_clock::now() - startTime).count() >=
                MAX_RACE_TIME) {
        finished = true;
        std::cout << "[RACE] RACE FINISHED! Reason: "
                  << (playerFinishTimes.size() == players.size() ? "All players done" :
                                                                   "Time limit")
                  << std::endl;
    }
}

void Race::updatePhysics(float dt) {
    // Actualizar física de los NPCs
    for (auto& npc : npcs) {
        if (!npc->isDestroyed()) {
            // Los NPCs tienen asignadas sus rutas, pasar nullptr
            npc->updatePhysics(dt, nullptr);
        }
    }
    
    physics.step(dt, VELOCITY_ITERATIONS);
    checkFinishConditions();
}

bool Race::isFinished() const { return finished; }

float Race::getCurrentElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(now - startTime).count();
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
        float speed = player->getSpeed();  // Get actual speed from player
        CarSnapshot cs{(uint8_t)player->getId(), pos.x, pos.y, false,
                       player->getCurrentHealth(), speed, angle,
                       player->isOnBridge(), player->getCarType()};
        cs.hasInfiniteHealth = player->hasInfiniteHealth();
        cs.isNPC = false;
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
        
        CarSnapshot cs{(uint8_t)(npcIdOffset + i), pos.x, pos.y, false,
                       npc->getCurrentHealth(), speed, angle, false, npc->getCarType()};
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
        std::cout << "CHEAT: Vida infinita activada para cliente: " << playerId << std::endl;
    }
}

void Race::forceWinCheat(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        // Marcar como finalizador inmediatamente
        auto now = std::chrono::steady_clock::now();
        auto raceTime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        playerFinishTimes[playerId] = static_cast<float>(raceTime);
        std::cout << "CHEAT: Cliente " << playerId << " ganó en " << raceTime << " segundos"
                  << std::endl;
    }
}

void Race::forceLoseCheat(int playerId) {
    auto it = std::find_if(
            players.begin(), players.end(),
            [playerId](const std::unique_ptr<Player>& p) { return p->getId() == playerId; });

    if (it != players.end()) {
        (*it)->getCar()->setDestroyed();
        std::cout << "CHEAT: Cliente " << playerId << " perdió automáticamente" << std::endl;
    }
}
