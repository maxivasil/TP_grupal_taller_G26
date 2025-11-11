#include "client_game.h"

#include <cmath>
#include <filesystem>

#include <SDL2pp/SDL2pp.hh>
#include <SDL_ttf.h>
#include <unistd.h>

#include "cmd/client_to_server_cheat.h"
#include "cmd/client_to_server_move.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FPS 1000 / 30

#define DATA_PATH "assets/"

Game::Game(ClientSession& client_session):
        client_session(client_session),
        camera(WINDOW_WIDTH, WINDOW_HEIGHT),
        minimap(150),
        hud(WINDOW_WIDTH, WINDOW_HEIGHT),
        hints(WINDOW_WIDTH, WINDOW_HEIGHT) {
    raceStartTime = SDL_GetTicks() / 1000.0f;  // Iniciar timer de carrera
}

int Game::start() {
    try {
        SDL2pp::SDL sdl(SDL_INIT_VIDEO);
        TTF_Init();  // Initialize SDL2_ttf

        std::string titulo = "NFS-2D";

        SDL2pp::Window window(titulo, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        init_textures(renderer);

        // Load HUD font
        std::vector<std::string> font_paths = {
                "assets/fonts/arial.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        };

        auto it = std::find_if(font_paths.begin(), font_paths.end(),
                               [](const auto& path) { return std::filesystem::exists(path); });

        if (it != font_paths.end()) {
            hud.loadFont(*it, 18);
            // Also pass the font to hints for cardinal labels
            hints.setHUDFont(new SDL2pp::Font(*it, 12), *it);
        }

        std::vector<std::string> map_paths = {
                "assets/cities/Liberty_City.png",
                "../assets/cities/Liberty_City.png",
                "../../assets/cities/Liberty_City.png",
        };

        auto it2 = std::find_if(map_paths.begin(), map_paths.end(),
                                [](const auto& path) { return std::filesystem::exists(path); });

        if (it2 != map_paths.end()) {
            std::cout << "Loading minimap from: " << *it2 << std::endl;
            minimap.loadMapImage(renderer, *it2);
            minimap.setWorldScale(62.0f / 8.9f, 24.0f / 3.086f);
            minimap.setZoomPixels(900.0f, 900.0f);
        }

        // Set up checkpoints for the race (Liberty City circuit)
        std::vector<RaceCheckpoint> checkpoints = {
                {0, 8.9f, 106.5f, 6.0f, 6.0f, false},   // Start checkpoint
                {1, 120.0f, 106.5f, 6.0f, 6.0f, true},  // Finish line
        };
        minimap.setCheckpoints(checkpoints);

        auto t1 = SDL_GetTicks();
        auto rate = FPS;

        Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();

        while (true) {
            t1 = SDL_GetTicks();  // t1 guarda el tiempo actual en milisegundos

            if (handleEvents(renderer)) {
                return 0;
            }

            ServerToClientCmd_Client* raw_cmd;
            while (recv_queue.try_pop(raw_cmd)) {
                std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);

                auto const* snapshot_cmd = dynamic_cast<ServerToClientSnapshot*>(cmd.get());
                if (snapshot_cmd) {
                    update(renderer, *snapshot_cmd);
                }
            }

            render(renderer);

            auto t2 = SDL_GetTicks();     // t2 guarda el tiempo actual en milisegundos
            int rest = rate - (t2 - t1);  // Tiempo que falta para la proxima iteracion.
                                          // Al tiempo buscado 'rate' se le resta
                                          // la diferencia entre 't2' y 't1', que es el tiempo que
                                          // se tardo en renderizar

            // Retrasado en comparacion al ritmo deseado
            if (rest < 0) {
                auto behind = -rest;                 // Tiempo de retraso
                auto lost = behind - behind % rate;  // Tiempo perdido
                t1 += lost;  // 't1' se adelanta en 'lost' milisegundos porque esos tiempos
                             // se perdieron a causa del retraso
            } else {         // A tiempo o adelantado
                SDL_Delay(rest);
            }

            t1 += rate;  // Aumentamos 't1' en 'rate' para la proxima iteracion
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
                    camera.setDimensions(renderer.GetOutputWidth(), renderer.GetOutputHeight());
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_m) {
                    showMinimap = !showMinimap;
                    std::cout << "Minimap: " << (showMinimap ? "ON" : "OFF") << "\n";
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
        client_session.send_command(new ClientToServerMove(MOVE_RIGHT));
    }
    if (state[SDL_SCANCODE_LEFT]) {
        client_session.send_command(new ClientToServerMove(MOVE_LEFT));
    }
    if (state[SDL_SCANCODE_UP]) {
        client_session.send_command(new ClientToServerMove(MOVE_UP));
    }
    if (state[SDL_SCANCODE_DOWN]) {
        client_session.send_command(new ClientToServerMove(MOVE_DOWN));
    }

    return false;
}

bool Game::update(SDL2pp::Renderer& renderer, ServerToClientSnapshot cmd_snapshot) {
    carsToRender.clear();
    ClientContext ctx = {.game = this, .mainwindow = nullptr};
    cmd_snapshot.execute(ctx);

    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                           [&](const CarSnapshot& car) { return car.id == client_id; });

    if (it != snapshots.end()) {
        float worldX = (it->pos_x * 62) / 8.9;
        float worldY = (it->pos_y * 24) / 3.086;
        camera.follow(worldX, worldY);

        // Actualizar hints hacia el próximo checkpoint
        if (currentCheckpoint < totalCheckpoints) {
            // Asumir que tenemos checkpoints en (8.9, 106.5) -> (120.0, 106.5)
            float checkpointX = (currentCheckpoint == 0) ? 8.9f : 120.0f;
            float checkpointY = 106.5f;
            // Pass player heading for camera-relative needle
            hints.updateHint(it->pos_x, it->pos_y, checkpointX, checkpointY, it->angle);

            // Detectar si pasamos el checkpoint (distancia < radio de 5 unidades)
            float dx = checkpointX - it->pos_x;
            float dy = checkpointY - it->pos_y;
            float distToCheckpoint = std::sqrt(dx * dx + dy * dy);

            const float CHECKPOINT_RADIUS = 5.0f;  // Radio de detección
            if (distToCheckpoint < CHECKPOINT_RADIUS && currentCheckpoint == 0) {
                // Pasamos el checkpoint de salida, ir al siguiente
                currentCheckpoint = 1;
                std::cout << "Checkpoint cruzado: " << currentCheckpoint << std::endl;
            } else if (distToCheckpoint < CHECKPOINT_RADIUS && currentCheckpoint == 1) {
                // Completamos la carrera - llamar al cheat de victoria automático
                std::cout << "Carrera completada!" << std::endl;
                setWon();
            }
        }
    }

    int texW = textures[0]->GetWidth();
    int texH = textures[0]->GetHeight();
    src = camera.getSrcRect(texW, texH);

    dst = {0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()};

    // Sprite del primer auto verde
    int carW = 28;
    int carH = 22;

    float scale = float(dst.w) / float(src.w);

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

    if (it != snapshots.end()) {
        float serverX = it->pos_x;
        float serverY = it->pos_y;

        std::cout << "SERVER: pos=(" << serverX << ", " << serverY << ")" << std::endl;


        localPlayer.x = serverX;
        localPlayer.y = serverY;
        localPlayer.angle = it->angle;
        localPlayer.playerId = client_id;
        localPlayer.health = it->health;
        localPlayer.isLocal = true;

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

    // Render HUD
    HUDData hudData;
    hudData.speed = 0.0f;  // Default to 0 if no local player
    hudData.health = 100.0f;

    // Use the first car's data (assumed to be the local player)
    if (!snapshots.empty()) {
        hudData.health = snapshots[0].health;

        // Calculate speed from position change if server speed is 0
        if (snapshots[0].speed > 0.0f) {
            hudData.speed = snapshots[0].speed;
        } else {
            // Client-side speed calculation based on position delta
            Uint32 currentTime = SDL_GetTicks();
            if (lastSpeedUpdateTime > 0) {
                float timeDelta =
                        (currentTime - lastSpeedUpdateTime) / 1000.0f;  // Convert to seconds
                if (timeDelta > 0.0f) {
                    float posX = snapshots[0].pos_x;
                    float posY = snapshots[0].pos_y;
                    float dx = posX - lastPlayerX;
                    float dy = posY - lastPlayerY;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    hudData.speed = distance / timeDelta;  // units per second
                }
            }
            lastPlayerX = snapshots[0].pos_x;
            lastPlayerY = snapshots[0].pos_y;
            lastSpeedUpdateTime = currentTime;
        }
    }

    hudData.checkpointCurrent = currentCheckpoint;
    hudData.checkpointTotal = totalCheckpoints;
    hudData.raceTime = (SDL_GetTicks() / 1000.0f) - raceStartTime;
    hud.render(renderer, hudData);

    // Render hints (directional arrow to checkpoint)
    if (!snapshots.empty()) {
        auto it2 = std::find_if(snapshots.begin(), snapshots.end(),
                                [&](const CarSnapshot& car) { return car.id == client_id; });
        if (it2 != snapshots.end()) {
            hints.render(renderer);
        }
    }

    // Render end game screen if game is over
    if (gameState != GameState::PLAYING) {
        renderEndGameScreen(renderer);
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
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    // Crear un rectángulo semi-transparente como fondo
    SDL_Rect bgRect = {0, 0, width, height};
    renderer.SetDrawColor(0, 0, 0, 200);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(bgRect);

    // Restaurar blend mode normal
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    // Título
    SDL2pp::Font titleFont(hud.fontPath.empty() ?
                                   "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" :
                                   hud.fontPath,
                           48);
    SDL_Color titleColor;
    std::string titleText;

    if (gameState == GameState::WON) {
        titleColor = {0, 255, 0, 255};
        titleText = "GANASTE!";
    } else {
        titleColor = {255, 0, 0, 255};
        titleText = "GAME OVER";
    }

    auto titleSurface = titleFont.RenderText_Solid(titleText, titleColor);
    SDL2pp::Texture titleTexture(renderer, titleSurface);

    int titleX = (width - titleTexture.GetWidth()) / 2;
    int titleY = height / 4;
    SDL_Rect titleRect = {titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()};
    renderer.Copy(titleTexture, SDL2pp::NullOpt, titleRect);

    // Mensaje adicional
    SDL2pp::Font msgFont(
            hud.fontPath.empty() ? "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" : hud.fontPath,
            24);
    SDL_Color msgColor = {255, 255, 255, 255};

    std::string msgText;
    msgText = "Presiona ESC para volver al lobby";

    auto msgSurface = msgFont.RenderText_Solid(msgText, msgColor);
    SDL2pp::Texture msgTexture(renderer, msgSurface);

    int msgX = (width - msgTexture.GetWidth()) / 2;
    int msgY = height / 2;
    SDL_Rect msgRect = {msgX, msgY, msgTexture.GetWidth(), msgTexture.GetHeight()};
    renderer.Copy(msgTexture, SDL2pp::NullOpt, msgRect);
}

void Game::setWon() {
    gameState = GameState::WON;
    endGameMessage = "¡GANASTE!";
    endGameTime = SDL_GetTicks();
    std::cout << "¡VICTORIA! Presiona ESC para volver al lobby\n";
}

void Game::setLost() {
    gameState = GameState::LOST;
    endGameMessage = "GAME OVER";
    endGameTime = SDL_GetTicks();
    std::cout << "DERROTA. Presiona ESC para volver al lobby\n";
}
