#include "Race.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "npc_route_from_track.h"

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
    // Use track-based system with increased NPC count
    initNPCsFromTrack(world);
}

void Race::initNPCsFromTrack(b2WorldId world) {
    // Legacy track-based NPC system (kept for fallback)
    // Usar los checkpoints del track como base para generar rutas de NPCs
    const auto& trackCheckpoints = track.getCheckpoints();
    const float CAR_WIDTH = 0.4f;  // Ancho de un auto (~40cm)

    if (trackCheckpoints.empty()) {
        std::cerr << "[RACE ERROR] No track checkpoints available for NPC route generation"
                  << std::endl;
        return;
    }

    std::cout << "[RACE] Generating NPC routes from " << trackCheckpoints.size()
              << " track checkpoints (fallback system)" << std::endl;

    // Convertir checkpoints del track a RoutePoint format
    std::vector<RoutePoint> trackRoutePoints;
    for (const auto& ckpt: trackCheckpoints) {
        trackRoutePoints.push_back(RoutePoint(ckpt.x, ckpt.y, 5.0f));
    }

    // Detectar número de checkpoints para adaptar distribución dinámicamente
    int numCheckpoints = trackCheckpoints.size();
    int numRoutes = 5;
    int npcsPerRoute = 3;
    int maxNPCs = 15;
    int maxParked = 5;

    // Ajustar según el tipo de track
    if (numCheckpoints < 6) {
        numRoutes = 4;
        npcsPerRoute = 4;
        maxNPCs = 16;
        maxParked = 5;
        std::cout << "[RACE NPC CONFIG] Small track detected (4 checkpoints): "
                  << maxNPCs << " moving, " << maxParked << " parked (well-spaced)" << std::endl;
    } else if (numCheckpoints >= 12) {
        numRoutes = 15;
        npcsPerRoute = 3;
        maxNPCs = 45;
        maxParked = 10;
        std::cout << "[RACE NPC CONFIG] Large track detected (12+ checkpoints): "
                  << maxNPCs << " moving across " << numRoutes << " routes (3 per route), " << maxParked 
                  << " parked (well-spaced)" << std::endl;
    }

    currentRoutes =
            NPCRouteFromTrack::generateRoutesFromTrackCheckpoints(trackRoutePoints, numRoutes);

    std::cout << "[RACE] Generated " << currentRoutes.size() << " NPC routes from track"
              << std::endl;

    // Distribuir NPCs: dinámicamente según el ancho de las calles (HYBRID APPROACH)
    int npcCount = 0;
    
    // Calcular ancho promedio de las calles
    float avgStreetWidth = 0.0f;
    for (const auto& ckpt : trackCheckpoints) {
        avgStreetWidth += ckpt.width;
    }
    avgStreetWidth /= trackCheckpoints.size();
    
    // Decidir si distribución es LATERAL o LONGITUDINAL
    // Force LONGITUDINAL distribution to avoid collisions (one car after another)
    bool lateralDistribution = false;
    
    // Calcular cuántos carriles de NPCs caben
    int numLanes = 1;
    if (lateralDistribution) {
        if (avgStreetWidth >= CAR_WIDTH * 3) {
            numLanes = 3;
        } else if (avgStreetWidth >= CAR_WIDTH * 2.5f) {
            numLanes = 2;
        }
    }
    
    std::cout << "[RACE NPC DISTRIBUTION] avgStreetWidth: " << avgStreetWidth 
              << " → Using " << (lateralDistribution ? "LATERAL" : "LONGITUDINAL") 
              << " distribution (numLanes: " << numLanes << ")" << std::endl;

    // GARANTIZAR AL MENOS 1 NPC EN CADA CHECKPOINT
    int checkpointsToFill = numCheckpoints;
    int remainingNPCs = maxNPCs;
    
    // Primero, distribuir NPCs para cubrir todos los checkpoints (1 por checkpoint aprox)
    for (int checkpointIdx = 0; checkpointIdx < checkpointsToFill && npcCount < maxNPCs; ++checkpointIdx) {
        if (remainingNPCs <= 0) break;
        
        // Seleccionar ruta aleatoria que pase por este checkpoint
        size_t routeIdx = checkpointIdx % currentRoutes.size();
        if (routeIdx >= currentRoutes.size()) continue;
        
        const auto& route = currentRoutes[routeIdx];
        if (route.points.empty()) continue;
        
        // Mapear checkpoint a waypoint en la ruta
        // Distribuir checkpoints a lo largo de la ruta de forma balanceada
        size_t routeLength = route.points.size();
        size_t waypointIdx = (routeLength * checkpointIdx) / checkpointsToFill;
        waypointIdx = std::min(waypointIdx, routeLength - 1);
        
        // Pequeña variación aleatoria para que no todos estén exactamente en el mismo punto
        int randomOffset = (rand() % 3) - 1;  // -1, 0, 1
        int finalWaypoint = std::max(0, std::min(static_cast<int>(routeLength - 1), 
                                                   static_cast<int>(waypointIdx) + randomOffset));
        
        const auto& point = route.points[finalWaypoint];
        b2Vec2 pos{point.x, point.y};
        uint8_t carType = rand() % 7;
        
        auto npc = std::make_unique<NPCTraffic>(world, carType, pos);
        npc->setRoute(&route.points);
        npcs.push_back(std::move(npc));
        
        std::cout << "[RACE] NPC #" << npcCount << " (moving, CHECKPOINT_COVERAGE) created at (" 
                  << pos.x << ", " << pos.y << ") on route " << (routeIdx + 1) 
                  << " [checkpoint: " << checkpointIdx << "/" << checkpointsToFill 
                  << "] [waypoint: " << finalWaypoint << "/" << routeLength 
                  << "] [Car Type: " << (int)carType << "]" << std::endl;
        
        npcCount++;
        remainingNPCs--;
    }
    
    // Distribuir NPCs restantes de forma balanceada
    for (size_t routeIdx = 0; routeIdx < currentRoutes.size() && npcCount < maxNPCs; ++routeIdx) {
        const auto& route = currentRoutes[routeIdx];
        if (route.points.empty())
            continue;

        size_t routeLength = route.points.size();
        
        // Agregar 2-3 NPCs adicionales por ruta para llenar espacios vacíos
        for (int extraIdx = 0; extraIdx < 3 && npcCount < maxNPCs; ++extraIdx) {
            // Distribuir en diferentes posiciones
            size_t basePointIdx = (routeLength * (extraIdx + 1)) / 5;  // Divide ruta en 5 secciones
            int randomOffset = (rand() % 3) - 1;
            int pointIdx = std::max(0, std::min(static_cast<int>(routeLength - 1), 
                                                static_cast<int>(basePointIdx) + randomOffset));

            const auto& point = route.points[pointIdx];
            b2Vec2 pos{point.x, point.y};
            uint8_t carType = rand() % 7;

            auto npc = std::make_unique<NPCTraffic>(world, carType, pos);
            npc->setRoute(&route.points);
            npcs.push_back(std::move(npc));

            std::cout << "[RACE] NPC #" << npcCount << " (moving, FILLER) created at (" 
                      << pos.x << ", " << pos.y << ") on route " << (routeIdx + 1) 
                      << " [waypoint: " << pointIdx << "/" << routeLength 
                      << "] [Car Type: " << (int)carType << "]" << std::endl;

            npcCount++;
        }
    }

    // Agregar autos estacionados - distribuidos aleatoriamente y lejanos del track
    std::vector<b2Vec2> parkedPositions;
    
    // Calcular límites del mapa
    float minX = 1e6f, maxX = -1e6f, minY = 1e6f, maxY = -1e6f;
    for (const auto& ckpt : trackCheckpoints) {
        minX = std::min(minX, ckpt.x - ckpt.width/2);
        maxX = std::max(maxX, ckpt.x + ckpt.width/2);
        minY = std::min(minY, ckpt.y - 50);
        maxY = std::max(maxY, ckpt.y + 50);
    }
    
    // Generar posiciones estacionadas LEJOS del track, en esquinas y lados
    float mapWidth = maxX - minX;
    float mapHeight = maxY - minY;
    
    // Distancia mínima del track
    float minDistFromTrack = std::max(mapWidth, mapHeight) * 0.2f;
    
    while (parkedPositions.size() < static_cast<size_t>(maxParked)) {
        // Seleccionar esquina o lado aleatorio (0-3)
        int region = rand() % 4;
        float parkedX, parkedY;
        
        switch (region) {
            case 0: // Top-left corner area
                parkedX = minX - minDistFromTrack + (rand() % 100) - 50;
                parkedY = minY - minDistFromTrack + (rand() % 100) - 50;
                break;
            case 1: // Top-right corner area
                parkedX = maxX + minDistFromTrack + (rand() % 100) - 50;
                parkedY = minY - minDistFromTrack + (rand() % 100) - 50;
                break;
            case 2: // Bottom-left corner area
                parkedX = minX - minDistFromTrack + (rand() % 100) - 50;
                parkedY = maxY + minDistFromTrack + (rand() % 100) - 50;
                break;
            case 3: // Bottom-right corner area
                parkedX = maxX + minDistFromTrack + (rand() % 100) - 50;
                parkedY = maxY + minDistFromTrack + (rand() % 100) - 50;
                break;
            default:
                parkedX = 0;
                parkedY = 0;
        }
        
        // Spacing entre autos estacionados (mínimo 20 unidades)
        bool tooClose = false;
        for (const auto& existing : parkedPositions) {
            float dist = std::sqrt((parkedX - existing.x) * (parkedX - existing.x) + 
                                  (parkedY - existing.y) * (parkedY - existing.y));
            if (dist < 20.0f) {
                tooClose = true;
                break;
            }
        }
        
        if (!tooClose) {
            parkedPositions.push_back(b2Vec2{parkedX, parkedY});
        }
    }

    int parkedCount = 0;
    for (const auto& parkedPos: parkedPositions) {
        if (parkedCount >= maxParked)
            break;

        uint8_t carType = rand() % 7;
        auto npc = std::make_unique<NPCTraffic>(world, carType, parkedPos);
        npcs.push_back(std::move(npc));

        std::cout << "[RACE] NPC #" << npcCount << " (PARKED) created at (" << parkedPos.x << ", "
                  << parkedPos.y << ") [Car Type: " << (int)carType << "]" << std::endl;

        npcCount++;
        parkedCount++;
    }

    std::cout << "[RACE] Initialized " << npcCount
              << " NPC traffic vehicles: " << (npcCount - parkedCount) << " moving across "
              << currentRoutes.size() << " routes, " << parkedCount << " parked" << std::endl;
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
    for (auto& npc: npcs) {
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
        CarSnapshot cs{(uint8_t)player->getId(),   pos.x, pos.y, false,
                       player->getCurrentHealth(), speed, angle, player->isOnBridge(),
                       player->getCarType()};
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
                       npc->getCurrentHealth(),    speed, angle, false,
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
