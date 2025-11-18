#include "client_game.h"

#include <cmath>
#include <filesystem>
#include <utility>

#include <SDL2pp/SDL2pp.hh>
#include <SDL_ttf.h>
#include <unistd.h>

#include "cmd/client_to_server_cheat.h"
#include "cmd/client_to_server_move.h"
#include "graphics/track_loader.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define PX_PER_METER_X (62.0f / 8.9f)
#define PX_PER_METER_Y (24.0f / 3.086f)

#define FPS 1000 / 30

#define DATA_PATH "assets/"


// Estructura de información de sprite
struct SpriteInfo {
    SDL_Rect rect;
    const char* name;
};

// Mapeo de sprites para los 7 autos en Cars_Combined.png
// Imagen: 196x22 píxeles (7 autos x 28 píxeles cada uno en una fila)
const SpriteInfo SPRITE_MAP[] = {
        {{0, 0, 75, 69}, "Iveco Daily"},         // Auto 0 - Van (75x69)
        {{0, 0, 75, 86}, "Dodge Viper"},         // Auto 1 - Ferrari (75x86)
        {{0, 0, 75, 86}, "Chevrolet Corsa"},     // Auto 2 - Celeste (75x86)
        {{0, 0, 75, 86}, "Jeep Wrangler"},       // Auto 3 - Jeep (75x86)
        {{0, 0, 75, 86}, "Toyota Tacoma"},       // Auto 4 - Pickup (75x86)
        {{0, 0, 75, 103}, "Lincoln TownCar"},    // Auto 5 - Limo (75x103)
        {{0, 0, 75, 86}, "Lamborghini Diablo"},  // Auto 6 - Descapotable (75x86)
};

const int NUM_SPRITES = 7;

Game::Game(ClientSession& client_session):
        client_session(client_session),
        camera(WINDOW_WIDTH, WINDOW_HEIGHT),
        minimap(150),
        hud(WINDOW_WIDTH, WINDOW_HEIGHT),
        arrow(WINDOW_WIDTH, WINDOW_HEIGHT) {
    raceStartTime = SDL_GetTicks() / 1000.0f;
    explosion.setSoundEngine(&carSoundEngine);
}

int Game::start() {
    try {
        SDL2pp::SDL sdl(SDL_INIT_VIDEO);
        TTF_Init();

        std::string titulo = "NFS-2D";

        SDL2pp::Window window(titulo, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

        SDL_Surface* icon = IMG_Load("assets/logo.png");
        if (icon) {
            SDL_SetWindowIcon(window.Get(), icon);
            SDL_FreeSurface(icon);
        } else {
            std::cerr << "No se pudo cargar el icono: " << SDL_GetError() << "\n";
        }

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        rendererPtr = &renderer;
        init_textures();

        std::vector<std::string> font_paths = {
                "assets/fonts/arial.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        };

        auto it = std::find_if(font_paths.begin(), font_paths.end(),
                               [](const auto& path) { return std::filesystem::exists(path); });

        if (it != font_paths.end()) {
            hud.loadFont(*it, 18);
        }


        auto t1 = SDL_GetTicks();
        auto rate = FPS;
        Uint32 prevTime = t1;

        Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();

        while (true) {
            t1 = SDL_GetTicks();
            float deltaTime = (t1 - prevTime) / 1000.0f;
            prevTime = t1;

            if (handleEvents()) {
                return 0;
            }

            ClientContext ctx;
            ctx.game = this;
            ctx.mainwindow = nullptr;

            ServerToClientCmd_Client* raw_cmd;
            while (recv_queue.try_pop(raw_cmd)) {
                std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);

                auto const* snapshot_cmd = dynamic_cast<ServerToClientSnapshot*>(cmd.get());
                if (snapshot_cmd) {
                    update(*snapshot_cmd);
                } else {
                    cmd->execute(ctx);
                }
            }

            explosion.update(deltaTime);

            render();

            auto t2 = SDL_GetTicks();
            int rest = rate - (t2 - t1);

            if (rest < 0) {
                auto behind = -rest;
                auto lost = behind - behind % rate;
                t1 += lost;
            } else {
                SDL_Delay(rest);
            }

            t1 += rate;
        }

        return 0;

    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return -1;
    }
}

bool Game::handleEvents() {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return true;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int w = rendererPtr->GetOutputWidth();
                    int h = rendererPtr->GetOutputHeight();
                    float uiScale = getScale(w, h);

                    camera.setDimensions(w, h);
                    minimap.onWindowResize(w, h, uiScale);
                    hud.onWindowResize(w, h, uiScale);
                    arrow.onWindowResize(w, h, uiScale);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_m) {
                    showMinimap = !showMinimap;
                } else if (event.key.keysym.sym == SDLK_t) {
                    if (gameState != GameState::PLAYING && raceFullyFinished &&
                        !accumulatedResults.empty()) {

                        if (endScreenPhase == EndScreenPhase::RACE_RESULTS) {
                            endScreenPhase = EndScreenPhase::ACCUMULATED_RESULTS;
                        } else {
                            endScreenPhase = EndScreenPhase::RACE_RESULTS;
                        }
                    }
                }
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_ESCAPE] || state[SDL_SCANCODE_Q]) {
        return true;
    }

    // CHEATS
    if (state[SDL_SCANCODE_LCTRL]) {
        if (state[SDL_SCANCODE_H]) {
            client_session.send_command(new ClientToServerCheat(CHEAT_INFINITE_HEALTH));
            carSoundEngine.playCheatActivated();
            std::cout << "CHEAT: Vida infinita activada\n";
        } else if (state[SDL_SCANCODE_W]) {
            client_session.send_command(new ClientToServerCheat(CHEAT_WIN));
            std::cout << "CHEAT: Victoria automática\n";
            setWon();
        } else if (state[SDL_SCANCODE_L]) {
            client_session.send_command(new ClientToServerCheat(CHEAT_LOSE));
            std::cout << "CHEAT: Derrota automática\n";
            setLost();
        }
    }

    if (state[SDL_SCANCODE_RIGHT]) {
        if (!playerDestroyed)
            client_session.send_command(new ClientToServerMove(MOVE_RIGHT));
    }
    if (state[SDL_SCANCODE_LEFT]) {
        if (!playerDestroyed)
            client_session.send_command(new ClientToServerMove(MOVE_LEFT));
    }
    if (state[SDL_SCANCODE_UP]) {
        if (!playerDestroyed)
            client_session.send_command(new ClientToServerMove(MOVE_UP));
    }
    if (state[SDL_SCANCODE_DOWN]) {
        if (!playerDestroyed)
            client_session.send_command(new ClientToServerMove(MOVE_DOWN));
    }

    return false;
}

bool Game::update(ServerToClientSnapshot cmd_snapshot) {
    carsToRender.clear();
    ClientContext ctx = {.game = this, .mainwindow = nullptr};
    cmd_snapshot.execute(ctx);

    int texW = textures[currentCityId]->GetWidth();
    int texH = textures[currentCityId]->GetHeight();
    src = camera.getSrcRect(texW, texH);

    dst = {0, 0, rendererPtr->GetOutputWidth(), rendererPtr->GetOutputHeight()};
    float scale = float(dst.w) / float(src.w);

    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                           [&](const CarSnapshot& car) { return car.id == client_id; });

    if (it != snapshots.end()) {
        float worldX = it->pos_x * PX_PER_METER_X;
        float worldY = it->pos_y * PX_PER_METER_Y;
        camera.follow(worldX, worldY);

        float previousHealth = previousHealthState[it->id];

        if (previousHealth == 0.0f && it->health > 0.0f) {
            previousHealth = it->health;
        }

        if (it->health < previousHealth && previousHealth > 0.0f) {
            float healthDamage = previousHealth - it->health;
            lastCollisionIntensity = healthDamage;

            if (healthDamage > 5.0f) {
                collisionFlashStartTime = SDL_GetTicks();
            }

            explosion.trigger(worldX, worldY, src.x, src.y, scale);
        }

        if (it->health <= 0.0f && !playerDestroyed) {
            playerDestroyed = true;
            destructionStartTime = SDL_GetTicks();
            std::cout << "[GAME] Player destroyed! Health: " << it->health << std::endl;
        }

        // If destroyed, trigger death sequence after short delay
        if (playerDestroyed && (SDL_GetTicks() - destructionStartTime) > 500) {
            setLost();
        }

        previousHealthState[it->id] = it->health;

        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        bool isAccelerating = keyState[SDL_SCANCODE_UP];
        bool isBraking = keyState[SDL_SCANCODE_DOWN];
        bool isTurning = keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_RIGHT];

        carSoundEngine.update(isAccelerating, isTurning, isBraking);

        if (currentCheckpoint < totalCheckpoints) {
            const RaceCheckpoint& current = trackCheckpoints[currentCheckpoint];
            float checkpointX = current.x;
            float checkpointY = current.y;

            arrow.updateTarget(it->pos_x, it->pos_y, checkpointX, checkpointY, it->angle);

            float halfW = current.width / 2.0f;
            float halfH = current.height / 2.0f;
            float minX = checkpointX - halfW;
            float maxX = checkpointX + halfW;
            float minY = checkpointY - halfH;
            float maxY = checkpointY + halfH;

            float px = it->pos_x;
            float py = it->pos_y;

            if (px >= minX && px <= maxX && py >= minY && py <= maxY) {
                if (currentCheckpoint == totalCheckpoints - 1) {
                    setWon();
                } else {
                    currentCheckpoint++;
                }
            }
        }
    }

    // Función helper para obtener tamaño según car_type
    auto getCarSize = [](uint8_t car_type) -> std::pair<int, int> {
        switch (car_type) {
            case 0:
                return {28, 22};  // Van (75x69) - proporción: 28/75 = 0.373
            case 1:
                return {40, 24};  // Ferrari (75x86) - proporción: 28/75 = 0.373, 32/86 = 0.372
            case 2:
                return {39, 24};  // Celeste (75x86)
            case 3:
                return {38, 24};  // Jeep (75x86)
            case 4:
                return {40, 22};  // Pickup (75x86)
            case 5:
                return {48, 20};  // Limo (75x103) - proporción: 28/75 = 0.373, 39/103 = 0.379
            case 6:
                return {40, 22};  // Descapotable (75x86)
            default:
                return {28, 22};
        }
    };

    // Get player position for distance calculations
    float playerWorldX = 0.0f, playerWorldY = 0.0f;
    if (it != snapshots.end()) {
        playerWorldX = it->pos_x * PX_PER_METER_X;
        playerWorldY = it->pos_y * PX_PER_METER_Y;
    }

    for (const auto& car: snapshots) {
        float worldX = car.pos_x * PX_PER_METER_X;
        float worldY = car.pos_y * PX_PER_METER_Y;

        float relX = (worldX - src.x) * scale;
        float relY = (worldY - src.y) * scale;

        // Obtener tamaño según car_type
        auto [carW, carH] = getCarSize(car.car_type);

        RenderCar rc;
        rc.dst = {int(relX - (carW * scale) / 2), int(relY - (carH * scale) / 2), int(carW * scale),
                  int(carH * scale)};

        // Usar sprite correcto según car_type (auto elegido), no car.id (usuario)
        rc.src = getSpriteForCarId(car.car_type);
        rc.angle = car.angle;      // Restar 270 grados para orientación correcta
        rc.car_id = car.car_type;  // Usar car_type para renderizado

        if (car.id == client_id) {
            camera.follow(worldX, worldY);
        } else {
            float distToOtherCar = std::sqrt((worldX - playerWorldX) * (worldX - playerWorldX) +
                                             (worldY - playerWorldY) * (worldY - playerWorldY));

            float prevHealth = otherPlayersLastHealth[car.id];
            if (prevHealth == 0.0f && car.health > 0.0f) {
                prevHealth = car.health;  // First time
            }

            if (car.health < prevHealth && prevHealth > 0.0f) {
                float healthDamage = prevHealth - car.health;
                carSoundEngine.playOtherCarCollision(distToOtherCar, healthDamage);
                std::cout << "[GAME] Other car (ID: " << car.id << ") collided at distance "
                          << distToOtherCar << std::endl;
            }
            otherPlayersLastHealth[car.id] = car.health;

            float prevSpeed = otherPlayersLastSpeed[car.id];
            if (car.speed < prevSpeed && prevSpeed > 5.0f && car.speed < 2.0f) {
                carSoundEngine.playDistantBrake(distToOtherCar);
            }
            otherPlayersLastSpeed[car.id] = car.speed;
        }

        rc.onBridge = car.onBridge;
        rc.hasInfiniteHealth = car.hasInfiniteHealth;  // Asignar el estado de vida infinita
        carsToRender.push_back(rc);
    }

    return false;  // Aca quizas haya que devolver true una vez que termine la partida?
}

void Game::render() {
    rendererPtr->SetDrawColor(0, 0, 0, 255);
    rendererPtr->Clear();

    rendererPtr->Copy(*textures[currentCityId], src, dst);
    for (const auto& rc: carsToRender) {
        if (!rc.onBridge) {
            // Usar textura individual del auto según su car_id
            if (carTextures.count(rc.car_id)) {
                rendererPtr->Copy(*carTextures[rc.car_id], rc.src, rc.dst, rc.angle,
                                  SDL2pp::NullOpt, SDL_FLIP_NONE);
            }
        }
    }
    rendererPtr->Copy(*textures[currentCityId + 3], src, dst);
    for (const auto& rc: carsToRender) {
        if (rc.onBridge) {
            // Usar textura individual del auto según su car_id
            if (carTextures.count(rc.car_id)) {
                rendererPtr->Copy(*carTextures[rc.car_id], rc.src, rc.dst, rc.angle,
                                  SDL2pp::NullOpt, SDL_FLIP_NONE);
            }
        }
    }

    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                           [&](const CarSnapshot& car) { return car.id == client_id; });

    MinimapPlayer localPlayer;
    std::vector<MinimapPlayer> otherPlayers;
    HUDData hudData;
    hudData.speed = 0.0f;
    hudData.health = 100.0f;

    if (it != snapshots.end()) {
        float serverX = it->pos_x;
        float serverY = it->pos_y;

        localPlayer.x = serverX;
        localPlayer.y = serverY;
        localPlayer.angle = it->angle;
        localPlayer.playerId = client_id;
        localPlayer.health = it->health;
        localPlayer.isLocal = true;

        hudData.health = it->health;
        if (it->speed > 0.0f) {
            hudData.speed = it->speed;
        } else {
            Uint32 currentTime = SDL_GetTicks();
            if (lastSpeedUpdateTime > 0) {
                float timeDelta = (currentTime - lastSpeedUpdateTime) / 1000.0f;

                if (timeDelta > 0.0f) {
                    float dx = it->pos_x - lastPlayerX;
                    float dy = it->pos_y - lastPlayerY;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    hudData.speed = distance / timeDelta;
                }
            }

            lastPlayerX = it->pos_x;
            lastPlayerY = it->pos_y;
            lastSpeedUpdateTime = currentTime;
        }

        for (const auto& car: snapshots) {
            if (car.id != client_id) {
                MinimapPlayer mp;
                mp.x = car.pos_x;  // Coordenadas del servidor
                mp.y = car.pos_y;  // Ajuste si es necesario
                mp.angle = car.angle;
                mp.playerId = car.id;
                mp.health = car.health;
                mp.isLocal = false;
                otherPlayers.push_back(mp);
            }
        }
    } else {
        // Sin datos reales - animar posición de test para demostración
        // Esto permite ver el minimap funcionando aunque no haya datos del servidor
        testPlayerX += 2.0f;
        testPlayerY += 1.5f;
        testPlayerAngle += 5.0f;
        if (testPlayerX > 700.0f)
            testPlayerX = 0.0f;
        if (testPlayerY > 600.0f)
            testPlayerY = 0.0f;
        if (testPlayerAngle >= 360.0f)
            testPlayerAngle = 0.0f;

        localPlayer.x = testPlayerX;
        localPlayer.y = testPlayerY;
        localPlayer.angle = testPlayerAngle;
        localPlayer.playerId = 0;
        localPlayer.health = 100.0f;
        localPlayer.isLocal = true;
    }

    if (showMinimap) {
        minimap.render(*rendererPtr, localPlayer, otherPlayers, currentCheckpoint);
    }

    renderCheckpoints(*rendererPtr);
    explosion.render(*rendererPtr);

    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    hudData.checkpointCurrent = currentCheckpoint;
    hudData.checkpointTotal = totalCheckpoints;
    hudData.raceTime = (SDL_GetTicks() / 1000.0f) - raceStartTime;
    hud.render(*rendererPtr, hudData);

    if (!snapshots.empty()) {
        auto it2 = std::find_if(snapshots.begin(), snapshots.end(),
                                [&](const CarSnapshot& car) { return car.id == client_id; });
        if (it2 != snapshots.end()) {
            arrow.render(*rendererPtr);
        }
    }

    if (gameState != GameState::PLAYING) {
        renderEndGameScreen();
    }

    // Render high-impact collision flash effect
    if (collisionFlashStartTime > 0) {
        Uint32 elapsedMs = SDL_GetTicks() - collisionFlashStartTime;
        if (elapsedMs < FLASH_DURATION_MS) {
            // Flash intensity fades from 1.0 to 0.0
            float progress = (float)elapsedMs / (float)FLASH_DURATION_MS;
            float intensity = 1.0f - progress;  // Fade out

            // Alternate between red and white based on collision intensity
            uint8_t flashAlpha =
                    static_cast<uint8_t>(255 * intensity * (lastCollisionIntensity / 20.0f));

            Uint8 prevR, prevG, prevB, prevA;
            rendererPtr->GetDrawColor(prevR, prevG, prevB, prevA);

            int width = rendererPtr->GetOutputWidth();
            int height = rendererPtr->GetOutputHeight();
            SDL_Rect flashRect = {0, 0, width, height};

            // Alternate: strong impacts = white, medium impacts = red
            if (lastCollisionIntensity > 10.0f) {
                rendererPtr->SetDrawColor(255, 255, 255,
                                          flashAlpha);  // White flash for high impact
            } else {
                rendererPtr->SetDrawColor(255, 80, 80, flashAlpha);  // Red flash for medium impact
            }

            rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            rendererPtr->FillRect(flashRect);
            rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);
            rendererPtr->SetDrawColor(prevR, prevG, prevB, prevA);
        } else {
            collisionFlashStartTime = 0;  // Stop flashing
        }
    }

    // Dibujar indicador de vida infinita en los bordes de la pantalla
    auto it_local = std::find_if(snapshots.begin(), snapshots.end(),
                                 [&](const CarSnapshot& car) { return car.id == client_id; });
    if (it_local != snapshots.end() && it_local->hasInfiniteHealth) {
        int windowWidth = rendererPtr->GetOutputWidth();
        int windowHeight = rendererPtr->GetOutputHeight();
        const int borderWidth = 10;  // Grosor del borde indicador (engrosado)

        rendererPtr->SetDrawColor(0, 255, 100, 255);  // Verde brillante

        // Borde superior
        SDL_Rect topBorder = {0, 0, windowWidth, borderWidth};
        rendererPtr->FillRect(topBorder);

        // Borde inferior
        SDL_Rect bottomBorder = {0, windowHeight - borderWidth, windowWidth, borderWidth};
        rendererPtr->FillRect(bottomBorder);

        // Borde izquierdo
        SDL_Rect leftBorder = {0, 0, borderWidth, windowHeight};
        rendererPtr->FillRect(leftBorder);

        // Borde derecho
        SDL_Rect rightBorder = {windowWidth - borderWidth, 0, borderWidth, windowHeight};
        rendererPtr->FillRect(rightBorder);
    }

    rendererPtr->Present();
}

void Game::renderCheckpoints(SDL2pp::Renderer& renderer) {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    float scaleX = float(dst.w) / float(src.w);
    float scaleY = float(dst.h) / float(src.h);

    size_t lastIndex = trackCheckpoints.size() - 1;
    for (size_t i = currentCheckpoint; i < trackCheckpoints.size(); ++i) {
        const RaceCheckpoint& cp = trackCheckpoints[i];

        float worldX = cp.x * PX_PER_METER_X;
        float worldY = cp.y * PX_PER_METER_Y;

        float worldW = cp.width * PX_PER_METER_X;
        float worldH = cp.height * PX_PER_METER_Y;
        float screenX = (worldX - src.x) * scaleX;
        float screenY = (worldY - src.y) * scaleY;

        float screenW = worldW * scaleX;
        float screenH = worldH * scaleY;

        SDL_Rect rect{int(screenX - screenW / 2), int(screenY - screenH / 2), int(screenW),
                      int(screenH)};

        if (i == static_cast<size_t>(currentCheckpoint) && i != lastIndex) {
            renderer.SetDrawColor(0, 255, 0, 130);
        } else if (i == lastIndex) {
            renderer.SetDrawColor(255, 0, 0, 140);
        } else {
            int dist = int(i - (currentCheckpoint + 1));
            int alpha = std::max(0, 130 - dist * 60);
            renderer.SetDrawColor(0, 255, 0, alpha);
        }
        renderer.FillRect(rect);
    }
}

void Game::init_textures() {
    textures[0] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr,
            SDL2pp::Surface(DATA_PATH "cities/Liberty_City.png").SetColorKey(true, 0));
    textures[3] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr,
            SDL2pp::Surface(DATA_PATH "cities/Liberty_City_bridges.png").SetColorKey(true, 0));
    textures[1] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr, SDL2pp::Surface(DATA_PATH "cities/San_Andreas.png").SetColorKey(true, 0));
    textures[4] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr,
            SDL2pp::Surface(DATA_PATH "cities/San_Andreas_bridges.png").SetColorKey(true, 0));
    textures[2] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr, SDL2pp::Surface(DATA_PATH "cities/Vice_City.png").SetColorKey(true, 0));
    textures[5] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr,
            SDL2pp::Surface(DATA_PATH "cities/Vice_City_bridges.png").SetColorKey(true, 0));

    // Cargar cada auto desde su PNG individual
    std::vector<std::pair<uint8_t, std::string>> carFiles = {
            {0, "iveco_daily.png"},        {1, "dodge_viper.png"},   {2, "chevrolet_corsa.png"},
            {3, "jeep_wrangler.png"},      {4, "toyota_tacoma.png"}, {5, "lincoln_towncar.png"},
            {6, "lamborghini_diablo.png"},
    };

    for (const auto& [car_id, filename]: carFiles) {
        std::string filepath = std::string(DATA_PATH) + "cars/" + filename;
        try {
            carTextures[car_id] = std::make_shared<SDL2pp::Texture>(
                    *rendererPtr, SDL2pp::Surface(filepath).SetColorKey(
                                          true, SDL_MapRGB(SDL2pp::Surface(filepath).Get()->format,
                                                           163, 163, 13)));
            std::cout << "[TEXTURES] Loaded car " << (int)car_id << ": " << filename << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[WARNING] Failed to load car texture " << filename << ": " << e.what()
                      << std::endl;
        }
    }
}

SDL_Rect Game::getSpriteForCarId(uint8_t car_id) {
    if (car_id >= NUM_SPRITES) {
        // Si el ID es inválido, usar el primer sprite por defecto
        return SPRITE_MAP[0].rect;
    }
    return SPRITE_MAP[car_id].rect;
}

void Game::update_snapshots(const std::vector<CarSnapshot>& snapshots) {
    this->snapshots = snapshots;
}

void Game::renderEndGameScreen() {
    Uint8 r, g, b, a;
    rendererPtr->GetDrawColor(r, g, b, a);
    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    SDL_Rect bgRect = {0, 0, width, height};
    rendererPtr->SetDrawColor(0, 0, 0, 200);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    rendererPtr->FillRect(bgRect);

    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);

    SDL2pp::Font titleFont(hud.fontPath.empty() ?
                                   "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" :
                                   hud.fontPath,
                           48);
    SDL_Color titleColor =
            (gameState == GameState::WON) ? SDL_Color{0, 255, 0, 255} : SDL_Color{255, 0, 0, 255};

    std::string titleText = (gameState == GameState::WON) ? "GANASTE!" : "GAME OVER";

    auto titleSurface = titleFont.RenderText_Solid(titleText, titleColor);
    SDL2pp::Texture titleTexture(*rendererPtr, titleSurface);

    int titleX = (width - titleTexture.GetWidth()) / 2;
    int titleY = height / 10;
    SDL_Rect titleRect = {titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()};
    rendererPtr->Copy(titleTexture, SDL2pp::NullOpt, titleRect);

    if (!raceFullyFinished) {
        renderMyOwnTime();
        renderPressESC();
        return;
    }

    if (raceFullyFinished) {
        if (endScreenPhase == EndScreenPhase::ACCUMULATED_RESULTS && !accumulatedResults.empty()) {

            renderAccumulatedTable();
            renderPressESC();
            return;
        } else {
            endScreenPhase = EndScreenPhase::RACE_RESULTS;
            renderRaceTable();
            renderPressESC();
            return;
        }
    }
}

void Game::setClientId(uint8_t id) {
    client_id = id;
    std::cout << "[GAME] Client ID set to " << (int)client_id << std::endl;
}

void Game::setWon() {
    if (gameState != GameState::PLAYING)
        return;
    gameState = GameState::WON;
    endGameMessage = "¡GANASTE!";
    endGameTime = SDL_GetTicks();
    carSoundEngine.playRaceFinish();
    std::cout << "¡VICTORIA! Presiona ESC para volver al lobby\n";
}

void Game::setLost() {
    if (gameState != GameState::PLAYING)
        return;
    gameState = GameState::LOST;
    endGameMessage = "GAME OVER";
    endGameTime = SDL_GetTicks();
    carSoundEngine.playGameOver();
    std::cout << "DERROTA. Presiona ESC para volver al lobby\n";
}

void Game::setRaceResults(const std::vector<ClientPlayerResult>& results, bool isFinished) {
    hasRaceResults = true;
    raceFullyFinished = isFinished;
    std::cout << "\n=== GAME::setRaceResults LLAMADO ===" << std::endl;
    std::cout << "Resultados de carrera recibidos: " << results.size() << " jugadores" << std::endl;

    auto it = std::find_if(results.begin(), results.end(),
                           [this](const auto& r) { return r.playerId == client_id; });

    if (it == results.end()) {
        return;
    }

    myOwnResults = *it;

    if (myOwnResults.finishTime < 0.0f && gameState == GameState::PLAYING) {
        std::cout << "[INFO] finishTime < 0 => DNF para player " << (int)client_id << std::endl;
        myOwnResults.position = 0;
        setLost();
        return;
    }

    if (!isFinished) {
        if (myOwnResults.position == 1)
            setWon();
        else
            setLost();
        return;
    }

    raceResults = results;

    if (myOwnResults.position == 1)
        setWon();
    else
        setLost();
}

float Game::getScale(int w, int h) const {
    float sx = float(w) / float(WINDOW_WIDTH);
    float sy = float(h) / float(WINDOW_HEIGHT);
    return std::min(sx, sy);
}

void Game::renderPressESC() {
    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    SDL2pp::Font instructFont(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            16);

    SDL_Color instructColor = {200, 200, 200, 255};

    std::string instructText;
    // cppcheck-suppress knownConditionTrueFalse
    if (!raceFullyFinished) {
        instructText = "Esperando a que terminen todos los jugadores...";
    } else if (accumulatedResults.empty()) {
        instructText = "Presiona ESC para volver al lobby";
    } else {
        if (endScreenPhase == EndScreenPhase::RACE_RESULTS)
            instructText = "T: ver acumulado | ESC: volver al lobby";
        else
            instructText = "T: ver resultado de esta carrera | ESC: volver al lobby";
    }

    auto instructSurface = instructFont.RenderText_Solid(instructText, instructColor);
    SDL2pp::Texture instructTexture(*rendererPtr, instructSurface);

    int instructX = (width - instructTexture.GetWidth()) / 2;
    int instructY = height - 50;

    SDL_Rect instructRect = {instructX, instructY, instructTexture.GetWidth(),
                             instructTexture.GetHeight()};

    rendererPtr->Copy(instructTexture, SDL2pp::NullOpt, instructRect);
}

void Game::renderMyOwnTime() {
    SDL_Color color = {255, 255, 255, 255};

    SDL2pp::Font font(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            28);

    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    int minutes = (int)myOwnResults.finishTime / 60;
    int seconds = (int)myOwnResults.finishTime % 60;
    int millis = (int)((myOwnResults.finishTime - (int)myOwnResults.finishTime) * 1000);

    char line[128];
    if (myOwnResults.position == 0) {
        snprintf(line, sizeof(line), "Tu tiempo: DNF (No completaste la carrera)");
    } else {
        snprintf(line, sizeof(line), "Tu tiempo: %02d:%02d.%03d (Puesto %d)", minutes, seconds,
                 millis, (int)myOwnResults.position);
    }
    auto surf = font.RenderText_Solid(line, color);
    SDL2pp::Texture tex(*rendererPtr, surf);

    int x = (width - tex.GetWidth()) / 2;
    int y = height / 2;

    SDL_Rect rect = {x, y, tex.GetWidth(), tex.GetHeight()};
    rendererPtr->Copy(tex, SDL2pp::NullOpt, rect);
}

void Game::renderRaceTable() {
    SDL2pp::Font font(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            20);

    SDL_Color headerColor = {255, 255, 0, 255};
    SDL_Color rowColor = {200, 200, 200, 255};

    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    int startY = height / 4;
    int lineHeight = 28;

    auto surfHeader = font.RenderText_Solid("RESULTADOS", headerColor);
    SDL2pp::Texture texHeader(*rendererPtr, surfHeader);

    SDL_Rect headerRect = {(width - texHeader.GetWidth()) / 2, startY, texHeader.GetWidth(),
                           texHeader.GetHeight()};
    rendererPtr->Copy(texHeader, SDL2pp::NullOpt, headerRect);
    startY += lineHeight + 15;

    std::vector<ClientPlayerResult> finished, dnf;

    for (const auto& r: raceResults) {
        if (r.finishTime >= 0)
            finished.push_back(r);
        else
            dnf.push_back(r);
    }

    for (const auto& r: finished) {
        bool isMe = (r.playerId == client_id);

        int minutes = (int)r.finishTime / 60;
        int seconds = (int)r.finishTime % 60;
        int millis = (int)((r.finishTime - (int)r.finishTime) * 1000);

        char line[256];
        snprintf(line, sizeof(line), "%d. %s   %02d:%02d.%03d", (int)r.position,
                 r.playerName.c_str(), minutes, seconds, millis);

        auto surfRow = font.RenderText_Solid(line, rowColor);
        SDL2pp::Texture texRow(*rendererPtr, surfRow);

        SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                            texRow.GetHeight()};

        if (isMe) {
            SDL_Rect bgRect = {rowRect.x - 10, rowRect.y - 2, rowRect.w + 20, rowRect.h + 4};

            SDL_Color highlightColor = {255, 255, 100, 90};

            rendererPtr->SetDrawColor(highlightColor.r, highlightColor.g, highlightColor.b,
                                      highlightColor.a);
            rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            rendererPtr->FillRect(bgRect);

            rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);
            rendererPtr->SetDrawColor(255, 255, 255, 255);
        }

        rendererPtr->Copy(texRow, SDL2pp::NullOpt, rowRect);
        startY += lineHeight;
    }

    if (!dnf.empty()) {
        startY += 20;

        auto surfDNF = font.RenderText_Solid("NO COMPLETARON (DNF)", headerColor);
        SDL2pp::Texture texDNF(*rendererPtr, surfDNF);
        SDL_Rect dnfHeaderRect = {(width - texDNF.GetWidth()) / 2, startY, texDNF.GetWidth(),
                                  texDNF.GetHeight()};
        rendererPtr->Copy(texDNF, SDL2pp::NullOpt, dnfHeaderRect);

        startY += lineHeight;

        for (const auto& r: dnf) {
            bool isMe = (r.playerId == client_id);
            char line[256];
            snprintf(line, sizeof(line), "-    %s    DNF", r.playerName.c_str());

            auto surfRow = font.RenderText_Solid(line, rowColor);
            SDL2pp::Texture texRow(*rendererPtr, surfRow);

            SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                                texRow.GetHeight()};

            if (isMe) {
                SDL_Rect bgRect = {rowRect.x - 10, rowRect.y - 2, rowRect.w + 20, rowRect.h + 4};

                SDL_Color highlightColor = {255, 255, 100, 90};

                rendererPtr->SetDrawColor(highlightColor.r, highlightColor.g, highlightColor.b,
                                          highlightColor.a);
                rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
                rendererPtr->FillRect(bgRect);

                rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);
                rendererPtr->SetDrawColor(255, 255, 255, 255);
            }

            rendererPtr->Copy(texRow, SDL2pp::NullOpt, rowRect);
            startY += lineHeight;
        }
    }
}

void Game::renderAccumulatedTable() {
    SDL2pp::Font font(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            20);

    SDL_Color headerColor = {0, 200, 255, 255};
    SDL_Color rowColor = {200, 200, 200, 255};

    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    int startY = height / 4;
    int lineHeight = 28;

    auto surfHeader = font.RenderText_Solid("ACUMULADO DEL CIRCUITO", headerColor);
    SDL2pp::Texture texHeader(*rendererPtr, surfHeader);

    SDL_Rect headerRect = {(width - texHeader.GetWidth()) / 2, startY, texHeader.GetWidth(),
                           texHeader.GetHeight()};
    rendererPtr->Copy(texHeader, SDL2pp::NullOpt, headerRect);

    startY += lineHeight + 20;

    for (const auto& r: accumulatedResults) {
        bool isMe = (r.playerId == client_id);
        char line[256];

        if (r.completedRaces == 0) {
            snprintf(line, sizeof(line), "%u  |  Carreras: 0  |  Tiempo total: N/A", r.playerId);
        } else {
            snprintf(line, sizeof(line), "%u  |  Carreras: %u  |  Tiempo total: %.2f", r.playerId,
                     r.completedRaces, r.totalTime);
        }

        auto surfRow = font.RenderText_Solid(line, rowColor);
        SDL2pp::Texture texRow(*rendererPtr, surfRow);

        SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                            texRow.GetHeight()};

        if (isMe) {
            SDL_Rect bgRect = {rowRect.x - 10, rowRect.y - 2, rowRect.w + 20, rowRect.h + 4};

            SDL_Color highlightColor = {255, 255, 100, 90};

            rendererPtr->SetDrawColor(highlightColor.r, highlightColor.g, highlightColor.b,
                                      highlightColor.a);
            rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            rendererPtr->FillRect(bgRect);

            rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);
            rendererPtr->SetDrawColor(255, 255, 255, 255);
        }

        rendererPtr->Copy(texRow, SDL2pp::NullOpt, rowRect);
        startY += lineHeight;
    }
}

void Game::setAccumulatedResults(const std::vector<AccumulatedResultDTO>& res) {
    accumulatedResults = res;
}

void Game::resetForNextRace(uint8_t nextCityId, const std::string& trackName) {
    gameState = GameState::PLAYING;
    currentCityId = nextCityId;
    hasRaceResults = false;
    raceFullyFinished = false;
    endScreenPhase = EndScreenPhase::RACE_RESULTS;
    accumulatedResults.clear();
    raceResults.clear();
    myOwnResults = ClientPlayerResult();
    raceStartTime = SDL_GetTicks() / 1000.0f;
    currentCheckpoint = 0;
    playerDestroyed = false;
    previousHealthState.clear();
    otherPlayersLastHealth.clear();
    otherPlayersLastSpeed.clear();
    lastSpeedUpdateTime = 0;
    lastPlayerX = 0.0f;
    lastPlayerY = 0.0f;
    initMinimapAndCheckpoints(trackName);
    std::cout << "[GAME] Preparado para la siguiente carrera." << std::endl;
}

void Game::initMinimapAndCheckpoints(const std::string& trackName) {

    std::map<uint8_t, std::string> city_map_paths = {
            {0, "assets/cities/Liberty_City.png"},
            {1, "assets/cities/San_Andreas.png"},
            {2, "assets/cities/Vice_City.png"},
    };
    auto it2 = std::find_if(city_map_paths.begin(), city_map_paths.end(),
                            [this](const auto& path) { return currentCityId == path.first; });

    if (it2 != city_map_paths.end()) {
        minimap.loadMapImage(*rendererPtr, it2->second);
        minimap.setWorldScale(PX_PER_METER_X, PX_PER_METER_Y);
        minimap.setZoomPixels(900.0f, 900.0f);
    }

    trackCheckpoints = TrackLoader::loadTrackCheckpoints(trackName);
    totalCheckpoints = trackCheckpoints.size();
    minimap.setCheckpoints(trackCheckpoints);
}
