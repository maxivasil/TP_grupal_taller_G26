#include "client_game.h"

#include <cmath>

#include <unistd.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FPS 1000 / 30

#define DATA_PATH "assets/"

Game::Game(ClientSession& client_session):
        client_session(client_session), camera(WINDOW_WIDTH, WINDOW_HEIGHT), minimap(150) {
    minimap.loadCityData("cities/liberty_city.yaml");
}

int Game::start() {
    try {
        SDL2pp::SDL sdl(SDL_INIT_VIDEO);

        std::string titulo = "NFS-2D";

        SDL2pp::Window window(titulo, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        init_textures(renderer);

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

                auto* snapshot_cmd = dynamic_cast<ServerToClientSnapshot*>(cmd.get());
                if (snapshot_cmd) {
                    snapshot_cmd->execute(*this);
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
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_ESCAPE] || state[SDL_SCANCODE_Q]) {
            return true;
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
        if (state[SDL_SCANCODE_M]) {
            showMinimap = !showMinimap;
            std::cout << "Minimap: " << (showMinimap ? "ON" : "OFF") << "\n";
        }
    }
    return false;
}

bool Game::update(SDL2pp::Renderer& renderer, ServerToClientSnapshot cmd_snapshot) {
    carsToRender.clear();
    cmd_snapshot.execute(*this);

    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                           [&](const CarSnapshot& car) { return car.id == client_id; });

    if (it != snapshots.end()) {
        float worldX = (it->pos_x * 62) / 8.9;
        float worldY = (it->pos_y * 24) / 3.086;
        camera.follow(worldX, worldY);
    }

    int texW = textures[0]->GetWidth();
    int texH = textures[0]->GetHeight();
    src = camera.getSrcRect(texW, texH);

    dst = {0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()};

    // Sprite del primer auto verde
    int carW = textures[1]->GetWidth() / 12;
    int carH = textures[1]->GetHeight() / 16;

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
        rc.src = {0, 0, carW, carH};
        rc.angle = car.angle;

        if (car.id == client_id) {
            camera.follow(worldX, worldY);
        }
        carsToRender.push_back(rc);
    }

    return false;  // Aca quizas haya que devolver true una vez que termine la partida?
}

void Game::render(SDL2pp::Renderer& renderer) {
    renderer.Clear();

    renderer.Copy(*textures[0], src, dst);
    for (const auto& rc: carsToRender) {
        renderer.Copy(*textures[1], rc.src, rc.dst, rc.angle, SDL2pp::NullOpt, SDL_FLIP_NONE);
    }

    // Render minimap
    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                           [&](const CarSnapshot& car) { return car.id == client_id; });
    
    MinimapPlayer localPlayer;
    std::vector<MinimapPlayer> otherPlayers;
    
    if (it != snapshots.end()) {
        // Tenemos datos reales del servidor
        float worldX = (it->pos_x * 62) / 8.9;
        float worldY = (it->pos_y * 24) / 3.086;
        
        localPlayer.x = worldX;
        localPlayer.y = worldY;
        localPlayer.angle = it->angle;
        localPlayer.playerId = client_id;
        localPlayer.health = it->health;
        localPlayer.isLocal = true;
        
        // Otros jugadores
        for (const auto& car : snapshots) {
            if (car.id != client_id) {
                MinimapPlayer mp;
                mp.x = (car.pos_x * 62) / 8.9;
                mp.y = (car.pos_y * 24) / 3.086;
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
        testPlayerAngle += 5.0f;  // Rotar constantemente
        
        // Wrap around si llega al borde
        if (testPlayerX > 700.0f) testPlayerX = 0.0f;
        if (testPlayerY > 600.0f) testPlayerY = 0.0f;
        if (testPlayerAngle >= 360.0f) testPlayerAngle = 0.0f;
        
        localPlayer.x = testPlayerX;
        localPlayer.y = testPlayerY;
        localPlayer.angle = testPlayerAngle;  // IMPORTANTE: usar ángulo dinámico
        localPlayer.playerId = 0;
        localPlayer.health = 100.0f;
        localPlayer.isLocal = true;
    }
    
    // Render minimap solo si está habilitado (presiona M para toggle)
    if (showMinimap) {
        minimap.render(renderer, localPlayer, otherPlayers);
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
}

void Game::update_snapshots(const std::vector<CarSnapshot>& snapshots) {
    this->snapshots = snapshots;
}
