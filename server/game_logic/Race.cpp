#include "Race.h"
#include "npc_route_from_track.h"

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
    // Usar los checkpoints del track como base para generar rutas de NPCs
    const auto& trackCheckpoints = track.getCheckpoints();
    
    if (trackCheckpoints.empty()) {
        std::cerr << "[RACE ERROR] No track checkpoints available for NPC route generation" << std::endl;
        return;
    }
    
    std::cout << "[RACE] Generating NPC routes from " << trackCheckpoints.size() 
              << " track checkpoints" << std::endl;
    
    // Convertir checkpoints del track a RoutePoint format
    std::vector<RoutePoint> trackRoutePoints;
    for (const auto& ckpt : trackCheckpoints) {
        trackRoutePoints.push_back(RoutePoint(ckpt.x, ckpt.y, 5.0f));
    }
    
    // Generar 5 variaciones del track (menos que antes para evitar congestión)
    int numRoutes = 5;
    currentRoutes = NPCRouteFromTrack::generateRoutesFromTrackCheckpoints(trackRoutePoints, numRoutes);
    
    std::cout << "[RACE] Generated " << currentRoutes.size() << " NPC routes from track" << std::endl;
    
    // Distribuir NPCs: 4 NPCs por ruta (20 total para 5 rutas) - REDUCIDO AÚN MÁS
    // Los NPCs se distribuyen a lo largo de la RUTA COMPLETA, no todos juntos al inicio
    int npcCount = 0;
    int npcsPerRoute = 4;  // Cambié de 6 a 4
    int maxNPCs = 20;      // Cambié de 30 a 20
    
    for (size_t routeIdx = 0; routeIdx < currentRoutes.size(); ++routeIdx) {
        const auto& route = currentRoutes[routeIdx];
        if (route.points.empty()) continue;
        
        // Distribuir NPCs equitativamente a lo largo de TODA la ruta
        size_t routeLength = route.points.size();
        
        // Calcular espaciado para distribuir a lo largo de la ruta completa
        // Esto asegura que no todos empiezan juntos
        size_t spacing = (routeLength > npcsPerRoute) ? (routeLength / npcsPerRoute) : 1;
        
        for (int i = 0; i < npcsPerRoute && npcCount < maxNPCs; ++i) {
            // Calcular índice base: distribuir uniformemente en toda la ruta
            size_t basePointIdx = (i * spacing) % routeLength;
            
            // Agregar variación aleatoria pequeña para no estar exactamente en línea
            int randomOffset = (rand() % 3) - 1;  // ±1 punto
            size_t pointIdx = (basePointIdx + randomOffset + routeLength) % routeLength;
            
            const auto& point = route.points[pointIdx];
            
            // Pequeño offset perpendicular para no superponer exactamente
            float offsetX = (rand() % 8 - 4) * 0.1f;  // ±0.4
            float offsetY = (rand() % 8 - 4) * 0.1f;
            
            b2Vec2 pos{point.x + offsetX, point.y + offsetY};
            uint8_t carType = rand() % 7;  // 7 tipos de autos (0-6)
            
            auto npc = std::make_unique<NPCTraffic>(world, carType, pos);
            npc->setRoute(&route.points);  // Asignar la ruta al NPC
            npcs.push_back(std::move(npc));
            
            std::cout << "[RACE] NPC #" << npcCount << " (moving) created at (" << pos.x << ", " << pos.y 
                      << ") on route " << (routeIdx + 1) << " [spacing: " << spacing << " waypoints]"
                      << " [Car Type: " << (int)carType << "]" << std::endl;
            
            npcCount++;
        }
    }
    
    // AGREGAR AUTOS ESTACIONADOS
    // Colocar algunos autos sin ruta (estacionados) en posiciones estratégicas
    std::vector<b2Vec2> parkedPositions;
    
    // Usar checkpoints del track como ubicaciones base para autos estacionados
    if (trackCheckpoints.size() >= 2) {
        // Estacionar autos cerca de algunos checkpoints
        parkedPositions.push_back(b2Vec2{trackCheckpoints[0].x + 10, trackCheckpoints[0].y + 5});
        parkedPositions.push_back(b2Vec2{trackCheckpoints[trackCheckpoints.size() / 2].x - 8, 
                                         trackCheckpoints[trackCheckpoints.size() / 2].y - 5});
    }
    
    // Si hay más de 4 checkpoints, agregar más posiciones estacionadas
    if (trackCheckpoints.size() >= 4) {
        parkedPositions.push_back(b2Vec2{trackCheckpoints[trackCheckpoints.size() - 1].x + 5, 
                                         trackCheckpoints[trackCheckpoints.size() - 1].y + 8});
        parkedPositions.push_back(b2Vec2{trackCheckpoints[trackCheckpoints.size() / 4].x - 6, 
                                         trackCheckpoints[trackCheckpoints.size() / 4].y + 10});
    }
    
    // Crear NPCs estacionados
    int parkedCount = 0;
    for (const auto& parkedPos : parkedPositions) {
        if (npcCount >= maxNPCs + 4) break;  // Máximo 4 autos estacionados adicionales
        
        uint8_t carType = rand() % 7;
        auto npc = std::make_unique<NPCTraffic>(world, carType, parkedPos);
        // NO asignar ruta - esto los deja estacionados
        npcs.push_back(std::move(npc));
        
        std::cout << "[RACE] NPC #" << npcCount << " (PARKED) created at (" << parkedPos.x << ", " << parkedPos.y 
                  << ") [Car Type: " << (int)carType << "]" << std::endl;
        
        npcCount++;
        parkedCount++;
    }
    
    std::cout << "[RACE] Initialized " << npcCount << " NPC traffic vehicles: " 
              << (npcCount - parkedCount) << " moving across " << currentRoutes.size() 
              << " routes, " << parkedCount << " parked" << std::endl;
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
