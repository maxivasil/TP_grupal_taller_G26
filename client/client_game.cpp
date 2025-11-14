#include "client_game.h"

#include <cmath>
#include <filesystem>

#include <SDL2pp/SDL2pp.hh>
#include <SDL_ttf.h>
#include <unistd.h>

#include "cmd/client_to_server_cheat.h"
#include "cmd/client_to_server_finishRace.h"
#include "cmd/client_to_server_move.h"
#include "graphics/track_loader.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FPS 1000 / 30

#define DATA_PATH "assets/"

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

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        init_textures(renderer);

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

        std::vector<std::string> map_paths = {
                "assets/cities/Liberty_City.png",
                "../assets/cities/Liberty_City.png",
                "../../assets/cities/Liberty_City.png",
        };

        auto it2 = std::find_if(map_paths.begin(), map_paths.end(),
                                [](const auto& path) { return std::filesystem::exists(path); });

        if (it2 != map_paths.end()) {
            minimap.loadMapImage(renderer, *it2);
            minimap.setWorldScale(62.0f / 8.9f, 24.0f / 3.086f);
            minimap.setZoomPixels(900.0f, 900.0f);
        }

        std::string trackName = "track_four_checkpoints_vice_city";
        trackCheckpoints = TrackLoader::loadTrackCheckpoints(trackName);

        if (trackCheckpoints.empty()) {
            std::cerr << "ERROR: No checkpoints loaded! Using fallback..." << std::endl;
            trackCheckpoints = {
                    {0, 13.8f, 192.6f, 18.2f, 9.1f, false},
                    {1, 231.7f, 192.9f, 17.5f, 10.9f, false},
                    {2, 240.5f, 107.3f, 18.9f, 11.5f, false},
                    {3, 439.8f, 107.0f, 20.2f, 13.3f, true},
            };
        }

        totalCheckpoints = trackCheckpoints.size();
        minimap.setCheckpoints(trackCheckpoints);

        auto t1 = SDL_GetTicks();
        auto rate = FPS;
        Uint32 prevTime = t1;

        Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();

        while (true) {
            t1 = SDL_GetTicks();
            float deltaTime = (t1 - prevTime) / 1000.0f;
            prevTime = t1;

            if (handleEvents(renderer)) {
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
                    update(renderer, *snapshot_cmd);
                } else {
                    cmd->execute(ctx);
                }
            }

            explosion.update(deltaTime);

            render(renderer);

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

bool Game::handleEvents(SDL2pp::Renderer& renderer) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return true;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int w = renderer.GetOutputWidth();
                    int h = renderer.GetOutputHeight();
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

bool Game::update(SDL2pp::Renderer& renderer, ServerToClientSnapshot cmd_snapshot) {
    carsToRender.clear();
    ClientContext ctx = {.game = this, .mainwindow = nullptr};
    cmd_snapshot.execute(ctx);

    int texW = textures[0]->GetWidth();
    int texH = textures[0]->GetHeight();
    src = camera.getSrcRect(texW, texH);

    dst = {0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()};
    float scale = float(dst.w) / float(src.w);

    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                           [&](const CarSnapshot& car) { return car.id == client_id; });

    if (it != snapshots.end()) {
        float worldX = (it->pos_x * 62) / 8.9;
        float worldY = (it->pos_y * 24) / 3.086;
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

            float dx = checkpointX - it->pos_x;
            float dy = checkpointY - it->pos_y;
            float distToCheckpoint = std::sqrt(dx * dx + dy * dy);

            const float CHECKPOINT_RADIUS = 5.0f;  // Radio de detección
            if (distToCheckpoint < CHECKPOINT_RADIUS) {
                if (currentCheckpoint == totalCheckpoints - 1) {
                    setWon();

                    auto finishCmd = std::make_unique<ClientToServerFinishRace>();
                    client_session.send_command(finishCmd.release());
                } else {
                    currentCheckpoint++;
                }
            }
        }
    }

    int carW = 28;
    int carH = 22;

    // Get player position for distance calculations
    float playerWorldX = 0.0f, playerWorldY = 0.0f;
    if (it != snapshots.end()) {
        playerWorldX = (it->pos_x * 62) / 8.9;
        playerWorldY = (it->pos_y * 24) / 3.086;
    }

    for (const auto& car: snapshots) {
        float worldX = (car.pos_x * 62) / 8.9;
        float worldY = (car.pos_y * 24) / 3.086;

        float relX = (worldX - src.x) * scale;
        float relY = (worldY - src.y) * scale;

        RenderCar rc;
        rc.dst = {int(relX - (carW * scale) / 2), int(relY - (carH * scale) / 2), int(carW * scale),
                  int(carH * scale)};

        // TODO: usar sprite según id o dirección
        rc.src = {2, 5, carW, carH};
        rc.angle = car.angle;

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
        carsToRender.push_back(rc);
    }

    return false;  // Aca quizas haya que devolver true una vez que termine la partida?
}

void Game::render(SDL2pp::Renderer& renderer) {
    renderer.Clear();

    renderer.Copy(*textures[0], src, dst);
    for (const auto& rc: carsToRender) {
        if (!rc.onBridge) {
            renderer.Copy(*textures[1], rc.src, rc.dst, rc.angle, SDL2pp::NullOpt, SDL_FLIP_NONE);
        }
    }
    renderer.Copy(*textures[2], src, dst);
    for (const auto& rc: carsToRender) {
        if (rc.onBridge) {
            renderer.Copy(*textures[1], rc.src, rc.dst, rc.angle, SDL2pp::NullOpt, SDL_FLIP_NONE);
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
        minimap.render(renderer, localPlayer, otherPlayers, currentCheckpoint);
    }


    explosion.render(renderer);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    hudData.checkpointCurrent = currentCheckpoint;
    hudData.checkpointTotal = totalCheckpoints;
    hudData.raceTime = (SDL_GetTicks() / 1000.0f) - raceStartTime;
    hud.render(renderer, hudData);

    if (!snapshots.empty()) {
        auto it2 = std::find_if(snapshots.begin(), snapshots.end(),
                                [&](const CarSnapshot& car) { return car.id == client_id; });
        if (it2 != snapshots.end()) {
            arrow.render(renderer);
        }
    }

    if (gameState != GameState::PLAYING) {
        renderEndGameScreen(renderer);
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
            renderer.GetDrawColor(prevR, prevG, prevB, prevA);

            int width = renderer.GetOutputWidth();
            int height = renderer.GetOutputHeight();
            SDL_Rect flashRect = {0, 0, width, height};

            // Alternate: strong impacts = white, medium impacts = red
            if (lastCollisionIntensity > 10.0f) {
                renderer.SetDrawColor(255, 255, 255, flashAlpha);  // White flash for high impact
            } else {
                renderer.SetDrawColor(255, 80, 80, flashAlpha);  // Red flash for medium impact
            }

            renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            renderer.FillRect(flashRect);
            renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
            renderer.SetDrawColor(prevR, prevG, prevB, prevA);
        } else {
            collisionFlashStartTime = 0;  // Stop flashing
        }
    }

    renderer.Present();
}

void Game::init_textures(SDL2pp::Renderer& renderer) {
    textures[0] = std::make_shared<SDL2pp::Texture>(
            renderer, SDL2pp::Surface(DATA_PATH "cities/Liberty_City.png").SetColorKey(true, 0));
    textures[1] = std::make_shared<SDL2pp::Texture>(
            renderer,
            SDL2pp::Surface(DATA_PATH "cars/Cars.png")
                    .SetColorKey(
                            true,
                            SDL_MapRGB(SDL2pp::Surface(DATA_PATH "cars/Cars.png").Get()->format,
                                       163, 163, 13)));
    textures[2] = std::make_shared<SDL2pp::Texture>(
            renderer,
            SDL2pp::Surface(DATA_PATH "cities/Liberty_City_bridges.png").SetColorKey(true, 0));
}

void Game::update_snapshots(const std::vector<CarSnapshot>& snapshots) {
    this->snapshots = snapshots;
}

void Game::renderEndGameScreen(SDL2pp::Renderer& renderer) {
    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    SDL_Rect bgRect = {0, 0, width, height};
    renderer.SetDrawColor(0, 0, 0, 200);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(bgRect);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    SDL2pp::Font titleFont(hud.fontPath.empty() ?
                                   "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" :
                                   hud.fontPath,
                           48);
    SDL_Color titleColor =
            (gameState == GameState::WON) ? SDL_Color{0, 255, 0, 255} : SDL_Color{255, 0, 0, 255};

    std::string titleText = (gameState == GameState::WON) ? "GANASTE!" : "GAME OVER";

    auto titleSurface = titleFont.RenderText_Solid(titleText, titleColor);
    SDL2pp::Texture titleTexture(renderer, titleSurface);

    int titleX = (width - titleTexture.GetWidth()) / 2;
    int titleY = height / 10;
    SDL_Rect titleRect = {titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()};
    renderer.Copy(titleTexture, SDL2pp::NullOpt, titleRect);

    if (hasRaceResults && !raceFullyFinished) {
        renderMyOwnTime(renderer, myOwnResults[0]);
        renderPressESC(renderer);
        return;
    }
    if (raceFullyFinished) {
        renderRaceTable(renderer, raceResults);
        renderPressESC(renderer);
        return;
    }
}

void Game::setClientId(uint8_t id) {
    client_id = id;
    std::cout << "[GAME] Client ID set to " << (int)client_id << std::endl;
}

void Game::setWon() {
    gameState = GameState::WON;
    endGameMessage = "¡GANASTE!";
    endGameTime = SDL_GetTicks();
    carSoundEngine.playRaceFinish();  // Play victory sound
    std::cout << "¡VICTORIA! Presiona ESC para volver al lobby\n";
}

void Game::setLost() {
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

    if (!isFinished) {
        myOwnResults.clear();

        auto it = std::find_if(results.begin(), results.end(),
                               [this](const auto& r) { return r.playerId == client_id; });

        if (it != results.end()) {
            myOwnResults.push_back(*it);
        } else {
            std::cerr << "[WARNING] PARCIAL recibido pero no contiene mi playerId="
                      << (int)client_id << std::endl;
        }
    } else {
        raceResults = results;
    }
}

float Game::getScale(int w, int h) const {
    float sx = float(w) / float(WINDOW_WIDTH);
    float sy = float(h) / float(WINDOW_HEIGHT);
    return std::min(sx, sy);
}

void Game::renderPressESC(SDL2pp::Renderer& renderer) {
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    SDL2pp::Font instructFont(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            16);

    SDL_Color instructColor = {200, 200, 200, 255};

    std::string instructText = "Presiona ESC para volver al lobby";

    auto instructSurface = instructFont.RenderText_Solid(instructText, instructColor);
    SDL2pp::Texture instructTexture(renderer, instructSurface);

    int instructX = (width - instructTexture.GetWidth()) / 2;
    int instructY = height - 50;

    SDL_Rect instructRect = {instructX, instructY, instructTexture.GetWidth(),
                             instructTexture.GetHeight()};

    renderer.Copy(instructTexture, SDL2pp::NullOpt, instructRect);
}

void Game::renderMyOwnTime(SDL2pp::Renderer& renderer, const ClientPlayerResult& result) {
    SDL_Color color = {255, 255, 255, 255};

    SDL2pp::Font font(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            28);

    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    int minutes = (int)result.finishTime / 60;
    int seconds = (int)result.finishTime % 60;
    int millis = (int)((result.finishTime - (int)result.finishTime) * 1000);

    char line[128];
    snprintf(line, sizeof(line), "Tu tiempo: %02d:%02d.%03d (Puesto %d)", minutes, seconds, millis,
             (int)result.position);

    auto surf = font.RenderText_Solid(line, color);
    SDL2pp::Texture tex(renderer, surf);

    int x = (width - tex.GetWidth()) / 2;
    int y = height / 2;

    SDL_Rect rect = {x, y, tex.GetWidth(), tex.GetHeight()};
    renderer.Copy(tex, SDL2pp::NullOpt, rect);
}

void Game::renderRaceTable(SDL2pp::Renderer& renderer,
                           const std::vector<ClientPlayerResult>& results) {
    SDL2pp::Font font(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            20);

    SDL_Color headerColor = {255, 255, 0, 255};
    SDL_Color rowColor = {200, 200, 200, 255};

    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    int startY = height / 4;
    int lineHeight = 28;

    auto surfHeader = font.RenderText_Solid("RESULTADOS", headerColor);
    SDL2pp::Texture texHeader(renderer, surfHeader);

    SDL_Rect headerRect = {(width - texHeader.GetWidth()) / 2, startY, texHeader.GetWidth(),
                           texHeader.GetHeight()};
    renderer.Copy(texHeader, SDL2pp::NullOpt, headerRect);
    startY += lineHeight + 15;

    for (const auto& r: results) {

        int minutes = (int)r.finishTime / 60;
        int seconds = (int)r.finishTime % 60;
        int millis = (int)((r.finishTime - (int)r.finishTime) * 1000);

        char line[256];
        snprintf(line, sizeof(line), "%d. %s   %02d:%02d.%03d", (int)r.position,
                 r.playerName.c_str(), minutes, seconds, millis);

        auto surfRow = font.RenderText_Solid(line, rowColor);
        SDL2pp::Texture texRow(renderer, surfRow);

        SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                            texRow.GetHeight()};

        renderer.Copy(texRow, SDL2pp::NullOpt, rowRect);
        startY += lineHeight;
    }
}
