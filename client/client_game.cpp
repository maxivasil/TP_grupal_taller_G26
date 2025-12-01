#include "client_game.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define ZOOM 1.5f
#define MINIMAP_SIZE 150
#define PX_PER_METER_X (62.0f / 8.9f)
#define PX_PER_METER_Y (24.0f / 3.086f)
#define TARGET_FPS 30
#define FRAME_TIME_MS (1000 / TARGET_FPS)
#define NUM_SPRITES 7


// Estructura de información de sprite
struct SpriteInfo {
    SDL_Rect rect;
    const char* name;
    std::pair<int, int> size;
};

const SpriteInfo SPRITE_MAP[] = {
        {{0, 0, 75, 69}, "Iveco Daily", {28, 22}},         // Auto 0 - Van (75x69)
        {{0, 0, 75, 86}, "Dodge Viper", {40, 24}},         // Auto 1 - Ferrari (75x86)
        {{0, 0, 75, 86}, "Chevrolet Corsa", {39, 24}},     // Auto 2 - Celeste (75x86)
        {{0, 0, 75, 86}, "Jeep Wrangler", {38, 24}},       // Auto 3 - Jeep (75x86)
        {{0, 0, 75, 86}, "Toyota Tacoma", {40, 22}},       // Auto 4 - Pickup (75x86)
        {{0, 0, 75, 103}, "Lincoln TownCar", {48, 20}},    // Auto 5 - Limo (75x103)
        {{0, 0, 75, 86}, "Lamborghini Diablo", {40, 22}},  // Auto 6 - Descapotable (75x86)
};

Game::Game(ClientSession& client_session):
        client_session(client_session),
        camera(WINDOW_WIDTH, WINDOW_HEIGHT, ZOOM),
        minimap(MINIMAP_SIZE),
        hud(WINDOW_WIDTH, WINDOW_HEIGHT),
        arrow(WINDOW_WIDTH, WINDOW_HEIGHT) {}

int Game::start() {
    try {
        SDL2pp::SDL sdl(SDL_INIT_VIDEO);
        TTF_Init();
        hud.initFont(fontPath, 18);

        std::string titulo = "NFS-2D";

        SDL2pp::Window window(titulo, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

        SDL_Surface* icon = IMG_Load(ABS_DIR ASSETS_DIR "logo.png");
        if (icon) {
            SDL_SetWindowIcon(window.Get(), icon);
            SDL_FreeSurface(icon);
        }

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        rendererPtr = &renderer;
        init_textures();

        auto t1 = SDL_GetTicks();
        Uint32 prevTime = t1;

        Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();

        while (true) {
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

                auto const* snapshot_cmd = dynamic_cast<ServerToClientSnapshot_Client*>(cmd.get());
                if (snapshot_cmd) {
                    update(*snapshot_cmd);
                } else {
                    cmd->execute(ctx);
                }
            }

            explosion.update(deltaTime);
            fireEffect.update(deltaTime);

            // Auto-close upgrades screen after duration
            if (showUpgradesScreen) {
                Uint32 elapsedMs = SDL_GetTicks() - upgradesScreenStartTime;
                if (elapsedMs >= UPGRADES_SCREEN_DURATION_MS) {
                    showUpgradesScreen = false;
                    selectedUpgrades = CarUpgrades();  // Reset
                }
            }

            render();

            auto t2 = SDL_GetTicks();
            int rest = FRAME_TIME_MS - (t2 - t1);

            if (rest < 0) {
                auto behind = -rest;
                auto lost = behind - (behind % FRAME_TIME_MS);
                t1 += lost;
            } else {
                SDL_Delay(rest);
            }

            t1 += FRAME_TIME_MS;
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
        if (showUpgradesScreen) {
            handleUpgradesInput(event);
        }

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
    if (state[SDL_SCANCODE_LCTRL] && !isCountdownActive()) {
        if (state[SDL_SCANCODE_H]) {
            client_session.send_command(new ClientToServerCheat_Client(CHEAT_INFINITE_HEALTH));
            carSoundEngine.playCheatActivated();
        } else if (state[SDL_SCANCODE_W]) {
            client_session.send_command(new ClientToServerCheat_Client(CHEAT_WIN));
            setWon();
        } else if (state[SDL_SCANCODE_L]) {
            client_session.send_command(new ClientToServerCheat_Client(CHEAT_LOSE));
            setLost();
        }
    }

    bool isAccelerating, isBraking, isTurning;

    if (state[SDL_SCANCODE_RIGHT]) {
        if (!playerDestroyed && !isCountdownActive()) {
            isTurning = true;
            client_session.send_command(new ClientToServerMove_Client(MOVE_RIGHT));
        }
    }
    if (state[SDL_SCANCODE_LEFT]) {
        if (!playerDestroyed && !isCountdownActive()) {
            isTurning = true;
            client_session.send_command(new ClientToServerMove_Client(MOVE_LEFT));
        }
    }
    if (state[SDL_SCANCODE_UP]) {
        if (!playerDestroyed && !isCountdownActive()) {
            isAccelerating = true;
            client_session.send_command(new ClientToServerMove_Client(MOVE_UP));
        }
    }
    if (state[SDL_SCANCODE_DOWN]) {
        if (!playerDestroyed && !isCountdownActive()) {
            isBraking = true;
            client_session.send_command(new ClientToServerMove_Client(MOVE_DOWN));
        }
    }

    if (gameState == GameState::PLAYING) {
        carSoundEngine.update(isAccelerating, isTurning, isBraking);
    }
    return false;
}

bool Game::update(ServerToClientSnapshot_Client cmd_snapshot) {
    carsToRender.clear();
    ClientContext ctx = {.game = this, .mainwindow = nullptr};
    cmd_snapshot.execute(ctx);

    int texW = textures[currentCityId]->GetWidth();
    int texH = textures[currentCityId]->GetHeight();
    src = camera.getSrcRect(texW, texH);

    dst = {0, 0, rendererPtr->GetOutputWidth(), rendererPtr->GetOutputHeight()};
    float scale = float(dst.w) / float(src.w);

    auto it = std::find_if(snapshots.begin(), snapshots.end(), [&](const CarSnapshot& car) {
        return !car.isNPC && car.id == client_id;
    });

    if (it != snapshots.end()) {
        float worldX = it->pos_x * PX_PER_METER_X;
        float worldY = it->pos_y * PX_PER_METER_Y;
        camera.follow(worldX, worldY);

        updateCarDamageState(*it, worldX, worldY, scale);
        updateCheckpointsState(it->pos_x, it->pos_y, it->angle);
    }

    for (const auto& car: snapshots) {
        float worldX = car.pos_x * PX_PER_METER_X;
        float worldY = car.pos_y * PX_PER_METER_Y;

        float relX = (worldX - src.x) * scale;
        float relY = (worldY - src.y) * scale;

        // Obtener tamaño según car_type
        auto [carW, carH] = SPRITE_MAP[car.car_type].size;

        RenderCar rc;
        rc.dst = {int(relX - (carW * scale) / 2), int(relY - (carH * scale) / 2), int(carW * scale),
                  int(carH * scale)};

        // Usar sprite correcto según car_type (auto elegido), no car.id (usuario)
        rc.src = getSpriteForCarId(car.car_type);
        rc.angle = car.angle;      // Restar 270 grados para orientación correcta
        rc.car_id = car.car_type;  // Usar car_type para renderizado

        rc.onBridge = car.onBridge;
        rc.hasInfiniteHealth = car.hasInfiniteHealth;  // Asignar el estado de vida infinita
        carsToRender.push_back(rc);
    }

    return false;
}

void Game::updateCarDamageState(const CarSnapshot& car, float worldX, float worldY, float scale) {
    float previousHealth = previousHealthState[car.id];

    if (previousHealth == 0.0f && car.health > 0.0f) {
        previousHealth = car.health;
    }

    if (car.health < previousHealth && previousHealth > 0.0f) {
        float healthDamage = previousHealth - car.health;
        lastCollisionIntensity = healthDamage;

        if (healthDamage > 5.0f) {
            collisionFlashStartTime = SDL_GetTicks();
        }

        carSoundEngine.playCollisionSound();
        explosion.trigger(worldX, worldY, src.x, src.y, scale);
    }

    if (car.health <= 0.0f && !playerDestroyed) {
        playerDestroyed = true;
        destructionStartTime = SDL_GetTicks();
        if (src.w > 0 && src.h > 0 && scale > 0) {
            carSoundEngine.playCollisionSound();
            explosion.triggerFinalExplosion(worldX, worldY, src.x, src.y, scale);
            float carWidth = src.w * 0.8f;
            float carHeight = src.h * 0.8f;
            if (carWidth > 0 && carHeight > 0 && carWidth < 1000 && carHeight < 1000) {
                float screenX = (worldX - src.x) * scale;
                float screenY = (worldY - src.y) * scale;
                fireEffect.start(screenX, screenY, carWidth, carHeight);
            }
        }
    }

    if (playerDestroyed && (SDL_GetTicks() - destructionStartTime) > 500) {
        setLost();
    }

    previousHealthState[car.id] = car.health;
}

void Game::updateCheckpointsState(float posX, float posY, float angle) {
    if (currentCheckpoint < totalCheckpoints) {
        const RaceCheckpoint& current = trackCheckpoints[currentCheckpoint];
        float checkpointX = current.x;
        float checkpointY = current.y;

        arrow.updateTarget(posX, posY, checkpointX, checkpointY, angle);

        float halfW = current.width / 2.0f;
        float halfH = current.height / 2.0f;
        float minX = checkpointX - halfW;
        float maxX = checkpointX + halfW;
        float minY = checkpointY - halfH;
        float maxY = checkpointY + halfH;

        float px = posX;
        float py = posY;

        if (px >= minX && px <= maxX && py >= minY && py <= maxY) {
            if (currentCheckpoint == totalCheckpoints - 1) {
                setWon();
            } else {
                currentCheckpoint++;
            }
        }
    }
}

void Game::render() {
    rendererPtr->SetDrawColor(0, 0, 0, 255);
    rendererPtr->Clear();
    rendererPtr->Copy(*textures[currentCityId], src, dst);
    renderCars(false);
    rendererPtr->Copy(*textures[currentCityId + 3], src, dst);
    renderCars(true);

    float scale = (src.w > 0) ? float(dst.w) / float(src.w) : 1.0f;
    renderPlayerNames(*rendererPtr, src, scale);

    auto it = std::find_if(snapshots.begin(), snapshots.end(), [&](const CarSnapshot& car) {
        return !car.isNPC && car.id == client_id;
    });

    MinimapPlayer localPlayer;
    HUDData hudData;
    hudData.speed = 0.0f;
    hudData.health = 100.0f;

    if (it != snapshots.end()) {
        localPlayer.x = it->pos_x;
        localPlayer.y = it->pos_y;
        localPlayer.angle = it->angle;
        hudData.health = it->health;
        hudData.speed = it->speed;
    }

    if (showMinimap) {
        minimap.render(*rendererPtr, localPlayer, currentCheckpoint);
    }

    renderCheckpoints(*rendererPtr);
    explosion.render(*rendererPtr);
    fireEffect.render(*rendererPtr);

    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    hudData.checkpointCurrent = currentCheckpoint;
    hudData.checkpointTotal = totalCheckpoints;
    hudData.raceTime = elapsedTime;
    hud.render(*rendererPtr, hudData);
    arrow.render(*rendererPtr);

    if (gameState != GameState::PLAYING) {
        renderEndGameScreen();
    }

    if (showUpgradesScreen) {
        renderUpgradesScreen();
    }

    renderFlashEffect();

    if (it != snapshots.end() && it->hasInfiniteHealth) {
        renderInfiniteHealthIndicator();
    }

    renderCountdown();

    rendererPtr->Present();
}

void Game::renderCars(bool onBridge) {
    for (const auto& rc: carsToRender) {
        if (rc.onBridge == onBridge) {
            if (carTextures.count(rc.car_id)) {
                rendererPtr->Copy(*carTextures[rc.car_id], rc.src, rc.dst, rc.angle,
                                  SDL2pp::NullOpt, SDL_FLIP_NONE);
            }
        }
    }
}

void Game::renderFlashEffect() {
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
}

void Game::renderInfiniteHealthIndicator() {
    int windowWidth = rendererPtr->GetOutputWidth();
    int windowHeight = rendererPtr->GetOutputHeight();
    const int borderWidth = 10;                   // Grosor del borde indicador (engrosado)
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
            SDL2pp::Surface(ABS_DIR ASSETS_DIR "cities/Liberty_City.png").SetColorKey(true, 0));
    textures[3] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr, SDL2pp::Surface(ABS_DIR ASSETS_DIR "cities/Liberty_City_bridges.png")
                                  .SetColorKey(true, 0));
    textures[1] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr,
            SDL2pp::Surface(ABS_DIR ASSETS_DIR "cities/San_Andreas.png").SetColorKey(true, 0));
    textures[4] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr, SDL2pp::Surface(ABS_DIR ASSETS_DIR "cities/San_Andreas_bridges.png")
                                  .SetColorKey(true, 0));
    textures[2] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr,
            SDL2pp::Surface(ABS_DIR ASSETS_DIR "cities/Vice_City.png").SetColorKey(true, 0));
    textures[5] = std::make_shared<SDL2pp::Texture>(
            *rendererPtr, SDL2pp::Surface(ABS_DIR ASSETS_DIR "cities/Vice_City_bridges.png")
                                  .SetColorKey(true, 0));

    // Cargar cada auto desde su PNG individual
    std::vector<std::pair<uint8_t, std::string>> carFiles = {
            {0, "iveco_daily.png"},        {1, "dodge_viper.png"},   {2, "chevrolet_corsa.png"},
            {3, "jeep_wrangler.png"},      {4, "toyota_tacoma.png"}, {5, "lincoln_towncar.png"},
            {6, "lamborghini_diablo.png"},
    };

    for (const auto& [car_id, filename]: carFiles) {
        std::string filepath = std::string(ABS_DIR) + std::string(ASSETS_DIR) + "cars/" + filename;
        carTextures[car_id] = std::make_shared<SDL2pp::Texture>(
                *rendererPtr,
                SDL2pp::Surface(filepath).SetColorKey(
                        true, SDL_MapRGB(SDL2pp::Surface(filepath).Get()->format, 163, 163, 13)));
    }
}

SDL_Rect Game::getSpriteForCarId(uint8_t car_id) {
    if (car_id >= NUM_SPRITES) {
        // Si el ID es inválido, usar el primer sprite por defecto
        return SPRITE_MAP[0].rect;
    }
    return SPRITE_MAP[car_id].rect;
}

void Game::update_snapshots(const std::vector<CarSnapshot>& snapshots, float elapsedTime) {
    this->snapshots = snapshots;
    this->elapsedTime = elapsedTime;
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

    SDL2pp::Font titleFont(fontPath, 48);
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

void Game::setClientId(uint32_t id) { client_id = id; }

void Game::setWon() {
    if (gameState != GameState::PLAYING)
        return;
    gameState = GameState::WON;
    endGameMessage = "¡GANASTE!";
    endGameTime = SDL_GetTicks();
    carSoundEngine.stopAll();  // Detener todos los sonidos de movimiento

    if (isLastRace) {
        carSoundEngine.playChampionshipWin();
    } else {
        carSoundEngine.playRaceFinish();
    }
}

void Game::setLost() {
    if (gameState != GameState::PLAYING)
        return;
    gameState = GameState::LOST;
    endGameMessage = "GAME OVER";
    endGameTime = SDL_GetTicks();
    carSoundEngine.stopAll();  // Detener todos los sonidos de movimiento
    carSoundEngine.playGameOver();
}

void Game::setRaceResults(const std::vector<ClientPlayerResult>& results, bool isFinished) {
    hasRaceResults = true;
    raceFullyFinished = isFinished;

    // Actualizar mapa de nombres
    for (const auto& r: results) {
        playerNames[r.playerId] = r.playerName;
    }

    auto it = std::find_if(results.begin(), results.end(),
                           [this](const auto& r) { return r.playerId == client_id; });

    if (it == results.end()) {
        return;
    }

    myOwnResults = *it;

    if (myOwnResults.finishTime < 0.0f && gameState == GameState::PLAYING) {
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

    // Activar pantalla de upgrades después de 1 segundo de que termine la carrera
    showUpgradesScreen = true;
    upgradesScreenStartTime = SDL_GetTicks();
    selectedUpgrades = CarUpgrades();  // Resetear selección
}

float Game::getScale(int w, int h) const {
    float sx = float(w) / float(WINDOW_WIDTH);
    float sy = float(h) / float(WINDOW_HEIGHT);
    return std::min(sx, sy);
}

void Game::renderPressESC() {
    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    SDL2pp::Font instructFont(fontPath, 16);

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

    SDL2pp::Font font(fontPath, 28);

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

void Game::renderOwnName(const SDL_Rect& rowRect) const {
    SDL_Rect bgRect = {rowRect.x - 10, rowRect.y - 2, rowRect.w + 20, rowRect.h + 4};
    SDL_Color highlightColor = {255, 255, 100, 90};
    rendererPtr->SetDrawColor(highlightColor.r, highlightColor.g, highlightColor.b,
                              highlightColor.a);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    rendererPtr->FillRect(bgRect);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);
    rendererPtr->SetDrawColor(255, 255, 255, 255);
}

void Game::renderRaceTable() {
    SDL2pp::Font font(fontPath, 20);

    SDL_Color headerColor = {255, 255, 0, 255};
    SDL_Color rowColor = {200, 200, 200, 255};

    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    int startY = height / 4;
    int lineHeight = 28;

    auto surfHeader = font.RenderText_Solid("RESULTADOS DE LA CARRERA", headerColor);
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
        snprintf(line, sizeof(line), "%2d. %-15.15s   %02d:%02d.%03ds", (int)r.position,
                 r.playerName.c_str(), minutes, seconds, millis);

        auto surfRow = font.RenderText_Solid(line, rowColor);
        SDL2pp::Texture texRow(*rendererPtr, surfRow);

        SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                            texRow.GetHeight()};

        if (isMe) {
            renderOwnName(rowRect);
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
            snprintf(line, sizeof(line), " --  %-15.15s   DNF", r.playerName.c_str());

            auto surfRow = font.RenderText_Solid(line, rowColor);
            SDL2pp::Texture texRow(*rendererPtr, surfRow);

            SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                                texRow.GetHeight()};

            if (isMe) {
                renderOwnName(rowRect);
            }

            rendererPtr->Copy(texRow, SDL2pp::NullOpt, rowRect);
            startY += lineHeight;
        }
    }
}

void Game::renderAccumulatedTable() {
    SDL2pp::Font font(fontPath, 20);

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

    int position = 1;

    for (const auto& r: accumulatedResults) {
        bool isMe = (r.playerId == client_id);
        char line[256];

        if (r.completedRaces == 0) {
            snprintf(line, sizeof(line), "%2d. %-15.15s | Carreras: 0 | T.Total: ---", position,
                     r.playerName.c_str());
        } else {
            snprintf(line, sizeof(line), "%2d. %-15.15s | Carreras: %u | T.Total: %7.2fs", position,
                     r.playerName.c_str(), r.completedRaces, r.totalTime);
        }

        auto surfRow = font.RenderText_Solid(line, rowColor);
        SDL2pp::Texture texRow(*rendererPtr, surfRow);

        SDL_Rect rowRect = {(width - texRow.GetWidth()) / 2, startY, texRow.GetWidth(),
                            texRow.GetHeight()};

        if (isMe) {
            renderOwnName(rowRect);
        }

        rendererPtr->Copy(texRow, SDL2pp::NullOpt, rowRect);
        startY += lineHeight;
        position++;
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
    currentCheckpoint = 0;
    playerDestroyed = false;
    explosion.stop();
    fireEffect.stop();
    previousHealthState.clear();
    initMinimapAndCheckpoints(trackName);
}

void Game::initMinimapAndCheckpoints(const std::string& trackName) {

    std::map<uint8_t, std::string> city_map_paths = {
            {0, ABS_DIR ASSETS_DIR "cities/Liberty_City.png"},
            {1, ABS_DIR ASSETS_DIR "cities/San_Andreas.png"},
            {2, ABS_DIR ASSETS_DIR "cities/Vice_City.png"},
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

void Game::renderUpgradesScreen() {
    if (isLastRace) {
        return;
    }
    Uint8 r, g, b, a;
    rendererPtr->GetDrawColor(r, g, b, a);
    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    // Fondo semi-transparente
    SDL_Rect bgRect = {0, 0, width, height};
    rendererPtr->SetDrawColor(0, 0, 0, 220);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    rendererPtr->FillRect(bgRect);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);

    // Título
    SDL2pp::Font titleFont(fontPath, 32);
    SDL_Color titleColor = {100, 200, 255, 255};  // Azul
    std::string titleText = "MEJORAS DISPONIBLES";
    auto titleSurface = titleFont.RenderText_Solid(titleText, titleColor);
    SDL2pp::Texture titleTexture(*rendererPtr, titleSurface);
    int titleX = (width - titleTexture.GetWidth()) / 2;
    int titleY = 30;
    SDL_Rect titleRect = {titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()};
    rendererPtr->Copy(titleTexture, SDL2pp::NullOpt, titleRect);

    // Fuentes
    SDL2pp::Font labelFont(fontPath, 18);
    SDL2pp::Font valueFont(fontPath, 16);

    SDL_Color labelColor = {200, 200, 200, 255};
    SDL_Color valueColor = {100, 255, 100, 255};    // Verde
    SDL_Color penaltyColor = {255, 100, 100, 255};  // Rojo para penalización

    // Dimensiones del upgrade display
    int centerX = width / 2;
    int startY = 100;
    int rowHeight = 50;

    // Array de upgrades labels
    const char* upgradeLabels[] = {"ACELERACION", "VELOCIDAD", "CONTROLABILIDAD", "SALUD"};
    const float* upgradeValues[] = {&selectedUpgrades.acceleration_boost,
                                    &selectedUpgrades.speed_boost, &selectedUpgrades.handling_boost,
                                    &selectedUpgrades.health_boost};
    const float penaltyFactors[] = {0.02f, 0.015f, 0.01f, 0.005f};

    // Render cada upgrade con botones +/-
    for (int i = 0; i < 4; i++) {
        int yPos = startY + (i * rowHeight);

        // Posicion del botón "-" para tener como referencia
        int minusX = centerX - 60;
        int minusY = yPos - 5;
        SDL_Rect minusRect = {minusX, minusY, 30, 30};

        // Label del upgrade
        auto labelSurface = labelFont.RenderText_Solid(upgradeLabels[i], labelColor);
        SDL2pp::Texture labelTexture(*rendererPtr, labelSurface);

        int labelX = minusX - 20 - labelTexture.GetWidth();

        SDL_Rect labelRect = {labelX, yPos, labelTexture.GetWidth(), labelTexture.GetHeight()};
        rendererPtr->Copy(labelTexture, SDL2pp::NullOpt, labelRect);

        // Botón "-"
        rendererPtr->SetDrawColor(200, 100, 100, 255);
        rendererPtr->FillRect(minusRect);

        auto minusSurface = labelFont.RenderText_Solid("-", {0, 0, 0, 255});
        SDL2pp::Texture minusTexture(*rendererPtr, minusSurface);
        int minusTextX = minusX + (30 - minusTexture.GetWidth()) / 2;
        int minusTextY = minusY + (30 - minusTexture.GetHeight()) / 2;
        SDL_Rect minusTextRect = {minusTextX, minusTextY, minusTexture.GetWidth(),
                                  minusTexture.GetHeight()};
        rendererPtr->Copy(minusTexture, SDL2pp::NullOpt, minusTextRect);

        // Valor actual
        std::string valueStr = std::to_string((int)(*upgradeValues[i]));
        auto valueSurface = valueFont.RenderText_Solid(valueStr.c_str(), valueColor);
        SDL2pp::Texture valueTexture(*rendererPtr, valueSurface);

        int valueX = centerX - (valueTexture.GetWidth() / 2);
        int valueY = yPos + 5;
        SDL_Rect valueRect = {valueX, valueY, valueTexture.GetWidth(), valueTexture.GetHeight()};
        rendererPtr->Copy(valueTexture, SDL2pp::NullOpt, valueRect);

        // Botón "+"
        int plusX = centerX + 60;
        int plusY = yPos - 5;
        SDL_Rect plusRect = {plusX, plusY, 30, 30};

        rendererPtr->SetDrawColor(100, 200, 100, 255);
        rendererPtr->FillRect(plusRect);

        auto plusSurface = labelFont.RenderText_Solid("+", {0, 0, 0, 255});
        SDL2pp::Texture plusTexture(*rendererPtr, plusSurface);
        int plusTextX = plusX + (30 - plusTexture.GetWidth()) / 2;
        int plusTextY = plusY + (30 - plusTexture.GetHeight()) / 2;
        SDL_Rect plusTextRect = {plusTextX, plusTextY, plusTexture.GetWidth(),
                                 plusTexture.GetHeight()};
        rendererPtr->Copy(plusTexture, SDL2pp::NullOpt, plusTextRect);

        // Mostrar tiempo de penalización
        float currentPenalty = (*upgradeValues[i]) * penaltyFactors[i];

        if (currentPenalty > 0.0001f) {
            std::stringstream ss;
            ss << "+" << std::fixed << std::setprecision(3) << currentPenalty << "s";

            auto penSurface = valueFont.RenderText_Solid(ss.str().c_str(), penaltyColor);
            SDL2pp::Texture penTexture(*rendererPtr, penSurface);

            int penX = plusX + 40;
            int penY = yPos + 5;
            SDL_Rect penRect = {penX, penY, penTexture.GetWidth(), penTexture.GetHeight()};
            rendererPtr->Copy(penTexture, SDL2pp::NullOpt, penRect);
        }
    }

    // Botón "APLICAR UPGRADES"
    int applyButtonY = startY + (4 * rowHeight) + 30;
    int applyButtonWidth = 200;
    int applyButtonHeight = 40;
    int applyButtonX = centerX - applyButtonWidth / 2;
    SDL_Rect applyRect = {applyButtonX, applyButtonY, applyButtonWidth, applyButtonHeight};
    rendererPtr->SetDrawColor(50, 150, 255, 255);  // Azul
    rendererPtr->FillRect(applyRect);
    rendererPtr->SetDrawColor(100, 200, 255, 255);
    rendererPtr->DrawRect(applyRect);

    auto applySurface = labelFont.RenderText_Solid("APLICAR UPGRADES", {0, 0, 0, 255});
    SDL2pp::Texture applyTexture(*rendererPtr, applySurface);
    int applyTextX = applyButtonX + (applyButtonWidth - applyTexture.GetWidth()) / 2;
    int applyTextY = applyButtonY + (applyButtonHeight - applyTexture.GetHeight()) / 2;
    SDL_Rect applyTextRect = {applyTextX, applyTextY, applyTexture.GetWidth(),
                              applyTexture.GetHeight()};
    rendererPtr->Copy(applyTexture, SDL2pp::NullOpt, applyTextRect);

    // Mostrar tiempo restante
    Uint32 elapsedMs = SDL_GetTicks() - upgradesScreenStartTime;
    int remainingSeconds = (UPGRADES_SCREEN_DURATION_MS - elapsedMs) / 1000;
    if (remainingSeconds < 0)
        remainingSeconds = 0;

    std::string timerStr = "Se cerrara en " + std::to_string(remainingSeconds) + " segundos";
    auto timerSurface = labelFont.RenderText_Solid(timerStr.c_str(), labelColor);
    SDL2pp::Texture timerTexture(*rendererPtr, timerSurface);
    int timerX = (width - timerTexture.GetWidth()) / 2;
    int timerY = height - 80;
    SDL_Rect timerRect = {timerX, timerY, timerTexture.GetWidth(), timerTexture.GetHeight()};
    rendererPtr->Copy(timerTexture, SDL2pp::NullOpt, timerRect);

    std::string instructText = "Usa los botones + y - para ajustar mejoras";
    auto instructSurface = labelFont.RenderText_Solid(instructText.c_str(), labelColor);
    SDL2pp::Texture instructTexture(*rendererPtr, instructSurface);
    int instructX = (width - instructTexture.GetWidth()) / 2;
    int instructY = height - 40;
    SDL_Rect instructRect = {instructX, instructY, instructTexture.GetWidth(),
                             instructTexture.GetHeight()};
    rendererPtr->Copy(instructTexture, SDL2pp::NullOpt, instructRect);
}

void Game::handleUpgradesInput(const SDL_Event& event) {
    if (event.type != SDL_MOUSEBUTTONDOWN)
        return;

    int mouseX = event.button.x;
    int mouseY = event.button.y;
    SDL_Point mousePoint = {mouseX, mouseY};

    int width = rendererPtr->GetOutputWidth();
    int centerX = width / 2;
    int startY = 100;
    int rowHeight = 50;

    // Array de valores de upgrades para modificar
    float* upgradeValues[] = {&selectedUpgrades.acceleration_boost, &selectedUpgrades.speed_boost,
                              &selectedUpgrades.handling_boost, &selectedUpgrades.health_boost};
    const float maxUpgrades = 100.0f;
    const float increment = 5.0f;  // Incrementos de 5 puntos por click

    // Verificar clics en botones +/-
    for (int i = 0; i < 4; i++) {
        int yPos = startY + (i * rowHeight);
        int minusX = centerX - 60;
        int minusY = yPos - 5;
        SDL_Rect minusRect = {minusX, minusY, 30, 30};

        if (SDL_PointInRect(&mousePoint, &minusRect)) {
            *upgradeValues[i] = std::max(0.0f, *upgradeValues[i] - increment);
            return;
        }

        int plusX = centerX + 60;
        int plusY = yPos - 5;
        SDL_Rect plusRect = {plusX, plusY, 30, 30};

        if (SDL_PointInRect(&mousePoint, &plusRect)) {
            *upgradeValues[i] = std::min(maxUpgrades, *upgradeValues[i] + increment);
            return;
        }
    }

    // Verificar clic en botón "APLICAR UPGRADES"
    int applyButtonY = startY + (4 * rowHeight) + 30;
    int applyButtonWidth = 200;
    int applyButtonHeight = 40;
    int applyButtonX = centerX - applyButtonWidth / 2;
    SDL_Rect applyRect = {applyButtonX, applyButtonY, applyButtonWidth, applyButtonHeight};
    if (SDL_PointInRect(&mousePoint, &applyRect)) {
        // Enviar comando al servidor
        client_session.send_command(new ClientToServerApplyUpgrades_Client(selectedUpgrades));

        // Cerrar pantalla de upgrades
        showUpgradesScreen = false;
        selectedUpgrades = CarUpgrades();  // Reset para la próxima vez

        return;
    }
}

void Game::renderCountdown() {
    if (!showCountdown) {
        return;
    }

    if (countdownValue == 0) {
        if (SDL_GetTicks() - yaStartTime > 1000) {
            showCountdown = false;
            return;
        }
    }

    Uint8 r, g, b, a;
    rendererPtr->GetDrawColor(r, g, b, a);

    int width = rendererPtr->GetOutputWidth();
    int height = rendererPtr->GetOutputHeight();

    // Semi-transparent background
    SDL_Rect bgRect = {0, 0, width, height};
    rendererPtr->SetDrawColor(0, 0, 0, 180);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    rendererPtr->FillRect(bgRect);
    rendererPtr->SetDrawBlendMode(SDL_BLENDMODE_NONE);

    // Render countdown text
    SDL2pp::Font countdownFont(fontPath, 80);  // Medium font

    SDL_Color textColor = {100, 200, 255, 255};  // Cyan/Sky blue
    std::string displayText;

    if (countdownValue > 0) {
        displayText = std::to_string(countdownValue);
    } else {
        displayText = "YA!";
    }

    auto textSurface = countdownFont.RenderText_Solid(displayText, textColor);
    SDL2pp::Texture textTexture(*rendererPtr, textSurface);

    int textX = (width - textTexture.GetWidth()) / 2;
    int textY = (height - textTexture.GetHeight()) / 2;
    SDL_Rect textRect = {textX, textY, textTexture.GetWidth(), textTexture.GetHeight()};

    rendererPtr->Copy(textTexture, SDL2pp::NullOpt, textRect);

    rendererPtr->SetDrawColor(r, g, b, a);
}

void Game::setCountdownValue(uint8_t value) {
    if (value == 0 && countdownValue != 0) {
        yaStartTime = SDL_GetTicks();
    }
    if (value == 3) {
        carSoundEngine.playCountdownRace();
    }

    countdownValue = value;
    showCountdown = true;
}

void Game::renderPlayerNames(SDL2pp::Renderer& renderer, const SDL_Rect& src, float scale) {
    if (snapshots.empty())
        return;

    SDL2pp::Font nameFont(fontPath, 14);

    for (const auto& car: snapshots) {
        if (car.isNPC || car.playerName.empty())
            continue;  // No mostrar nombres de NPCs ni autos sin nombre

        // Calcular posición en pantalla
        float worldX = car.pos_x * PX_PER_METER_X;
        float worldY = car.pos_y * PX_PER_METER_Y;
        float screenX = (worldX - src.x) * scale;
        float screenY = (worldY - src.y) * scale - 40;  // 40 píxeles arriba del auto

        // Crear texto blanco con fondo negro
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Color bgColor = {0, 0, 0, 150};
        auto textSurface = nameFont.RenderText_Solid(car.playerName, textColor);
        SDL2pp::Texture textTexture(renderer, textSurface);
        int textWidth = textTexture.GetWidth();
        int textHeight = textTexture.GetHeight();

        // Dibujar fondo negro con algo de padding
        int padding = 4;
        SDL_Rect bgRect = {int(screenX - textWidth / 2 - padding), int(screenY - padding),
                           textWidth + padding * 2, textHeight + padding * 2};
        renderer.SetDrawColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
        renderer.FillRect(bgRect);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

        // Dibujar texto
        SDL_Rect textRect = {int(screenX - textWidth / 2), int(screenY), textWidth, textHeight};
        renderer.Copy(textTexture, SDL2pp::NullOpt, textRect);
    }
}

bool Game::isCountdownActive() const { return showCountdown; }
