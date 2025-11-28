#include "Race.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
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
    initNPCsFromTrack(world);
}

void Race::initNPCsFromTrack(b2WorldId world) {
    // Sistema de spawn simplificado - sin rutas complejas
    // Los NPCs ahora spawnan en posiciones aleatorias y navegan usando lógica aleatoria
    
    const auto& trackCheckpoints = track.getCheckpoints();
    if (trackCheckpoints.empty()) {
        std::cerr << "[RACE ERROR] No track checkpoints available for NPC spawn" << std::endl;
        return;
    }

    // Calcular límites del mapa basado en checkpoints
    float minX = 1e6f, maxX = -1e6f, minY = 1e6f, maxY = -1e6f;
    for (const auto& ckpt : trackCheckpoints) {
        minX = std::min(minX, ckpt.x - ckpt.width / 2);
        maxX = std::max(maxX, ckpt.x + ckpt.width / 2);
        minY = std::min(minY, ckpt.y - 50);
        maxY = std::max(maxY, ckpt.y + 50);
    }

    // Expandir límites para que NPCs puedan spawnear alrededor
    float mapPadding = 100.0f;
    minX -= mapPadding;
    maxX += mapPadding;
    minY -= mapPadding;
    maxY += mapPadding;

    float mapWidth = maxX - minX;
    float mapHeight = maxY - minY;

    int numCheckpoints = trackCheckpoints.size();
    int maxNPCs = 100;  // Cantidad de NPCs móviles
    int maxParked = 45;  // Cantidad de NPCs estacionados

    // Ajustar según track
    if (numCheckpoints < 6) {
        maxNPCs = 55;
        maxParked = 10;
        std::cout << "[RACE NPC CONFIG] Small track: " << maxNPCs << " moving, " 
                  << maxParked << " parked" << std::endl;
    } else if (numCheckpoints >= 12) {
        maxNPCs = 150;
        maxParked = 85;
        std::cout << "[RACE NPC CONFIG] Large track: " << maxNPCs << " moving, " 
                  << maxParked << " parked" << std::endl;
    }

    int npcCount = 0;
    std::vector<b2Vec2> usedPositions;

    // ===== SPAWN NPCs MÓVILES =====
    for (int i = 0; i < maxNPCs; ++i) {
        b2Vec2 spawnPos;
        bool validPosition = false;
        int attempts = 0;

        // Intentar encontrar una posición válida (sin colisiones)
        while (!validPosition && attempts < 20) {
            // Generar posición aleatoria en el mapa
            float randomX = minX + (rand() % static_cast<int>(mapWidth));
            float randomY = minY + (rand() % static_cast<int>(mapHeight));
            spawnPos = b2Vec2{randomX, randomY};

            // Validar distancia mínima con otros NPCs
            validPosition = true;
            float MIN_SPAWN_DISTANCE = 40.0f;
            
            for (const auto& usedPos : usedPositions) {
                if (b2Distance(spawnPos, usedPos) < MIN_SPAWN_DISTANCE) {
                    validPosition = false;
                    break;
                }
            }

            attempts++;
        }

        // Si no encontró posición válida, pasar al siguiente
        if (!validPosition) {
            continue;
        }

        usedPositions.push_back(spawnPos);

        uint8_t carType = rand() % 7;
        auto npc = std::make_unique<NPCTraffic>(world, carType, spawnPos);
        npcs.push_back(std::move(npc));

        std::cout << "[RACE NPC] #" << npcCount << " (MOVING) spawned at (" 
                  << spawnPos.x << ", " << spawnPos.y << ") [Car Type: " 
                  << static_cast<int>(carType) << "]" << std::endl;
        npcCount++;
    }

    // ===== SPAWN NPCs ESTACIONADOS (esquinas del mapa) =====
    std::vector<b2Vec2> cornerPositions = {
        b2Vec2{minX + 50, minY + 50},      // Esquina superior izquierda
        b2Vec2{maxX - 50, minY + 50},      // Esquina superior derecha
        b2Vec2{minX + 50, maxY - 50},      // Esquina inferior izquierda
        b2Vec2{maxX - 50, maxY - 50},      // Esquina inferior derecha
    };

    int parkedCount = 0;
    for (const auto& cornerPos : cornerPositions) {
        if (parkedCount >= maxParked) break;

        // Agregar offset aleatorio a la esquina
        float offsetX = (rand() % 100) - 50;
        float offsetY = (rand() % 100) - 50;
        b2Vec2 parkedPos{cornerPos.x + offsetX, cornerPos.y + offsetY};

        uint8_t carType = rand() % 7;
        auto npc = std::make_unique<NPCTraffic>(world, carType, parkedPos);
        npc->setParked(true);
        npcs.push_back(std::move(npc));

        std::cout << "[RACE NPC] #" << npcCount << " (PARKED) spawned at (" 
                  << parkedPos.x << ", " << parkedPos.y << ") [Car Type: " 
                  << static_cast<int>(carType) << "]" << std::endl;
        npcCount++;
        parkedCount++;
    }

    std::cout << "[RACE] Initialized " << npcCount << " NPC traffic vehicles: " 
              << (npcCount - parkedCount) << " moving, " << parkedCount << " parked" << std::endl;
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
            // Actualizar física del NPC
            npc->updatePhysics(dt, nullptr);

            // ===== SISTEMA DE RESPAWN POR ATORAMIENTO (DESHABILITADO) =====
            // El respawn agresivo causa que NPCs desaparezcan del mapa visible.
            // Se detecta en updatePhysics de NPCTraffic: si stuckTimer > 1.5f, ya salta waypoints.
            // No es necesario respawnearlo aquí - dejar que NPCTraffic lo maneje.
            // 
            // Descomentar solo si NPCs se quedan completamente atrapados:
            // float npcSpeed = npc->getSpeed();
            // bool isStuck = (npcSpeed < 0.5f && npc->getStuckTimer() >= 5.0f);
            // if (isStuck && !npc->getParked()) { ... respawn logic ... }
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
        
        // Obtener el estado de puente del NPCCar subyacente
        bool onBridge = false;
        if (npc->getCar()) {
            onBridge = npc->getCar()->getIsOnBridge();
        }

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
