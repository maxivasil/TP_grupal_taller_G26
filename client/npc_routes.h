#ifndef NPC_ROUTES_H
#define NPC_ROUTES_H

#include <map>
#include <string>
#include <vector>

/**
 * @struct RoutePoint
 * @brief Un punto en una ruta de NPC
 */
struct RoutePoint {
    float x;
    float y;
    float speed;  // Velocidad en este segmento (m/s)

    RoutePoint(float x = 0.0f, float y = 0.0f, float speed = 3.5f): x(x), y(y), speed(speed) {}
};

/**
 * @struct SpawnPoint
 * @brief Un punto donde puede aparecer un NPC
 */
struct SpawnPoint {
    float x;
    float y;
    float direction;  // Ángulo inicial en grados (0-360)
    uint8_t car_type;  // Tipo de auto (0-6)
    int route_index;  // Índice de ruta a seguir (-1 = movimiento aleatorio)

    SpawnPoint(float x = 0.0f, float y = 0.0f, float direction = 0.0f, 
               uint8_t car_type = 0, int route_index = -1):
            x(x), y(y), direction(direction), car_type(car_type), route_index(route_index) {}
};

/**
 * @struct NPCRoute
 * @brief Una ruta completa que un NPC puede seguir
 */
struct NPCRoute {
    std::vector<RoutePoint> points;
    bool looping = true;  // ¿La ruta se repite?
    std::vector<SpawnPoint> spawn_points;  // Puntos de aparición de NPCs

    NPCRoute() = default;
    NPCRoute(const std::vector<RoutePoint>& pts, bool loop = true): points(pts), looping(loop) {}
};

/**
 * @class NPCRouteManager
 * @brief Gestor de rutas predeterminadas por CIUDAD
 * Las rutas son independientes de los checkpoints de la carrera
 * Siempre son los mismos recorridos en cada ciudad
 */
class NPCRouteManager {
private:
    std::map<std::string, std::vector<NPCRoute>> cityRoutes;

    void initializeSanAndreas() {
        std::vector<NPCRoute> routes;

        // RUTA 1: Autopista Este-Oeste (recorre TODO el mapa horizontalmente)
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(250.0f, 400.0f, 5.0f), RoutePoint(300.0f, 398.0f, 5.0f),
                    RoutePoint(350.0f, 402.0f, 5.0f), RoutePoint(400.0f, 400.0f, 5.1f),
                    RoutePoint(450.0f, 400.0f, 5.0f), RoutePoint(500.0f, 398.0f, 5.0f),
                    RoutePoint(550.0f, 402.0f, 5.0f), RoutePoint(600.0f, 400.0f, 5.0f),
                    RoutePoint(650.0f, 400.0f, 5.0f), RoutePoint(600.0f, 402.0f, 4.9f),
                    RoutePoint(550.0f, 402.0f, 4.9f), RoutePoint(500.0f, 402.0f, 5.0f),
                    RoutePoint(450.0f, 402.0f, 4.9f), RoutePoint(400.0f, 402.0f, 4.9f),
            };
            NPCRoute r(route, true);
            // Spawn points estratégicos en esta ruta
            r.spawn_points.push_back(SpawnPoint(280.0f, 400.0f, 0.0f, 0, 0));
            r.spawn_points.push_back(SpawnPoint(450.0f, 400.0f, 90.0f, 2, 0));
            r.spawn_points.push_back(SpawnPoint(600.0f, 400.0f, 180.0f, 1, 0));
            routes.emplace_back(r);
        }

        // RUTA 2: Autopista Norte-Sur (recorre verticalmente)
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(400.0f, 100.0f, 5.0f), RoutePoint(402.0f, 150.0f, 5.0f),
                    RoutePoint(400.0f, 200.0f, 5.1f), RoutePoint(398.0f, 250.0f, 5.0f),
                    RoutePoint(400.0f, 300.0f, 5.0f), RoutePoint(402.0f, 350.0f, 5.0f),
                    RoutePoint(400.0f, 400.0f, 5.0f), RoutePoint(398.0f, 450.0f, 5.0f),
                    RoutePoint(400.0f, 500.0f, 5.0f), RoutePoint(402.0f, 550.0f, 5.0f),
                    RoutePoint(400.0f, 600.0f, 4.9f), RoutePoint(398.0f, 550.0f, 4.9f),
                    RoutePoint(400.0f, 500.0f, 5.0f),
            };
            NPCRoute r(route, true);
            // Spawn points estratégicos en esta ruta
            r.spawn_points.push_back(SpawnPoint(400.0f, 150.0f, 90.0f, 1, 1));
            r.spawn_points.push_back(SpawnPoint(400.0f, 350.0f, 180.0f, 0, 1));
            r.spawn_points.push_back(SpawnPoint(400.0f, 550.0f, 270.0f, 2, 1));
            routes.emplace_back(r);
        }

        // RUTA 3: Diagonal Noroeste-Sureste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(260.0f, 150.0f, 4.8f), RoutePoint(310.0f, 200.0f, 4.8f),
                    RoutePoint(360.0f, 250.0f, 4.9f), RoutePoint(410.0f, 300.0f, 5.0f),
                    RoutePoint(460.0f, 350.0f, 4.9f), RoutePoint(510.0f, 400.0f, 4.8f),
                    RoutePoint(560.0f, 450.0f, 4.8f), RoutePoint(510.0f, 400.0f, 4.7f),
                    RoutePoint(460.0f, 350.0f, 4.8f), RoutePoint(410.0f, 300.0f, 4.9f),
                    RoutePoint(360.0f, 250.0f, 4.8f), RoutePoint(310.0f, 200.0f, 4.8f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 4: Diagonal Noreste-Suroeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(560.0f, 150.0f, 4.8f), RoutePoint(510.0f, 200.0f, 4.8f),
                    RoutePoint(460.0f, 250.0f, 4.9f), RoutePoint(410.0f, 300.0f, 5.0f),
                    RoutePoint(360.0f, 350.0f, 4.9f), RoutePoint(310.0f, 400.0f, 4.8f),
                    RoutePoint(260.0f, 450.0f, 4.8f), RoutePoint(310.0f, 400.0f, 4.7f),
                    RoutePoint(360.0f, 350.0f, 4.8f), RoutePoint(410.0f, 300.0f, 4.9f),
                    RoutePoint(460.0f, 250.0f, 4.8f), RoutePoint(510.0f, 200.0f, 4.8f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 5: Circuito Perimetral
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(270.0f, 150.0f, 4.5f), RoutePoint(330.0f, 120.0f, 4.6f),
                    RoutePoint(400.0f, 100.0f, 4.7f), RoutePoint(500.0f, 120.0f, 4.6f),
                    RoutePoint(550.0f, 180.0f, 4.6f), RoutePoint(580.0f, 250.0f, 4.5f),
                    RoutePoint(600.0f, 350.0f, 4.6f), RoutePoint(580.0f, 450.0f, 4.6f),
                    RoutePoint(500.0f, 500.0f, 4.7f), RoutePoint(400.0f, 520.0f, 4.6f),
                    RoutePoint(300.0f, 500.0f, 4.6f), RoutePoint(250.0f, 450.0f, 4.5f),
                    RoutePoint(240.0f, 350.0f, 4.6f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 6: Zig-Zag Oeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(290.0f, 200.0f, 4.2f), RoutePoint(300.0f, 270.0f, 4.2f),
                    RoutePoint(290.0f, 340.0f, 4.3f), RoutePoint(300.0f, 410.0f, 4.2f),
                    RoutePoint(290.0f, 480.0f, 4.2f), RoutePoint(310.0f, 410.0f, 4.1f),
                    RoutePoint(310.0f, 340.0f, 4.2f), RoutePoint(310.0f, 270.0f, 4.2f),
                    RoutePoint(310.0f, 200.0f, 4.1f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 7: Zig-Zag Este
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(530.0f, 200.0f, 4.2f), RoutePoint(520.0f, 270.0f, 4.2f),
                    RoutePoint(530.0f, 340.0f, 4.3f), RoutePoint(520.0f, 410.0f, 4.2f),
                    RoutePoint(530.0f, 480.0f, 4.2f), RoutePoint(510.0f, 410.0f, 4.1f),
                    RoutePoint(510.0f, 340.0f, 4.2f), RoutePoint(510.0f, 270.0f, 4.2f),
                    RoutePoint(510.0f, 200.0f, 4.1f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 8: Espiral Centro
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(390.0f, 290.0f, 4.0f), RoutePoint(430.0f, 310.0f, 4.0f),
                    RoutePoint(440.0f, 350.0f, 4.1f), RoutePoint(430.0f, 390.0f, 4.0f),
                    RoutePoint(390.0f, 410.0f, 4.0f), RoutePoint(355.0f, 390.0f, 4.1f),
                    RoutePoint(345.0f, 350.0f, 4.0f), RoutePoint(355.0f, 310.0f, 4.0f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 9: Pasillo Vertical Oeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(320.0f, 150.0f, 4.4f), RoutePoint(320.0f, 220.0f, 4.4f),
                    RoutePoint(320.0f, 290.0f, 4.5f), RoutePoint(320.0f, 360.0f, 4.4f),
                    RoutePoint(320.0f, 430.0f, 4.4f), RoutePoint(320.0f, 500.0f, 4.4f),
                    RoutePoint(340.0f, 430.0f, 4.3f), RoutePoint(340.0f, 360.0f, 4.3f),
                    RoutePoint(340.0f, 290.0f, 4.3f), RoutePoint(340.0f, 220.0f, 4.3f),
            };
            routes.emplace_back(route, true);
        }

        // RUTA 10: Pasillo Vertical Este
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(500.0f, 150.0f, 4.4f), RoutePoint(500.0f, 220.0f, 4.4f),
                    RoutePoint(500.0f, 290.0f, 4.5f), RoutePoint(500.0f, 360.0f, 4.4f),
                    RoutePoint(500.0f, 430.0f, 4.4f), RoutePoint(500.0f, 500.0f, 4.4f),
                    RoutePoint(480.0f, 430.0f, 4.3f), RoutePoint(480.0f, 360.0f, 4.3f),
                    RoutePoint(480.0f, 290.0f, 4.3f), RoutePoint(480.0f, 220.0f, 4.3f),
            };
            routes.emplace_back(route, true);
        }

        cityRoutes["san_andreas"] = routes;
    }

    void initializeLibertyCity() {
        std::vector<NPCRoute> routes;

        // RUTA 1: Autopista Este-Oeste (recorre todo el mapa horizontalmente)
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(300.0f, 350.0f, 5.0f),  // oeste
                    RoutePoint(350.0f, 348.0f, 5.0f), RoutePoint(400.0f, 352.0f, 5.1f),
                    RoutePoint(450.0f, 350.0f, 5.0f), RoutePoint(500.0f, 350.0f, 5.0f),
                    RoutePoint(550.0f, 348.0f, 5.0f), RoutePoint(600.0f, 352.0f, 5.0f),
                    RoutePoint(550.0f, 354.0f, 4.9f), RoutePoint(500.0f, 354.0f, 4.9f),
                    RoutePoint(450.0f, 354.0f, 5.0f), RoutePoint(400.0f, 354.0f, 4.9f),
                    RoutePoint(350.0f, 352.0f, 4.9f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(330.0f, 350.0f, 0.0f, 0, 0));
            r.spawn_points.push_back(SpawnPoint(450.0f, 350.0f, 0.0f, 1, 0));
            r.spawn_points.push_back(SpawnPoint(570.0f, 350.0f, 0.0f, 2, 0));
            routes.emplace_back(r);
        }

        // RUTA 2: Autopista Norte-Sur (recorre verticalmente)
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(400.0f, 250.0f, 5.0f),  // norte
                    RoutePoint(402.0f, 300.0f, 5.0f), RoutePoint(400.0f, 350.0f, 5.1f),
                    RoutePoint(398.0f, 400.0f, 5.0f), RoutePoint(400.0f, 450.0f, 5.0f),
                    RoutePoint(402.0f, 500.0f, 5.0f), RoutePoint(400.0f, 550.0f, 5.0f),
                    RoutePoint(398.0f, 500.0f, 4.9f), RoutePoint(400.0f, 450.0f, 4.9f),
                    RoutePoint(402.0f, 400.0f, 5.0f), RoutePoint(400.0f, 350.0f, 4.9f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(400.0f, 280.0f, 90.0f, 3, 1));
            r.spawn_points.push_back(SpawnPoint(400.0f, 400.0f, 90.0f, 4, 1));
            r.spawn_points.push_back(SpawnPoint(400.0f, 520.0f, 90.0f, 5, 1));
            routes.emplace_back(r);
        }

        // RUTA 3: Diagonal Noroeste-Sureste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(310.0f, 280.0f, 4.8f),  // noroeste
                    RoutePoint(350.0f, 320.0f, 4.8f), RoutePoint(390.0f, 360.0f, 4.9f),
                    RoutePoint(430.0f, 400.0f, 5.0f),  // centro
                    RoutePoint(470.0f, 440.0f, 4.9f), RoutePoint(510.0f, 480.0f, 4.8f),
                    RoutePoint(550.0f, 520.0f, 4.8f),  // sureste
                    RoutePoint(510.0f, 480.0f, 4.7f), RoutePoint(470.0f, 440.0f, 4.8f),
                    RoutePoint(430.0f, 400.0f, 4.9f), RoutePoint(390.0f, 360.0f, 4.8f),
                    RoutePoint(350.0f, 320.0f, 4.8f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(350.0f, 320.0f, 45.0f, 2, 2));
            r.spawn_points.push_back(SpawnPoint(430.0f, 400.0f, 45.0f, 0, 2));
            r.spawn_points.push_back(SpawnPoint(510.0f, 480.0f, 45.0f, 3, 2));
            routes.emplace_back(r);
        }

        // RUTA 4: Diagonal Noreste-Suroeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(550.0f, 280.0f, 4.8f),  // noreste
                    RoutePoint(510.0f, 320.0f, 4.8f), RoutePoint(470.0f, 360.0f, 4.9f),
                    RoutePoint(430.0f, 400.0f, 5.0f),  // centro
                    RoutePoint(390.0f, 440.0f, 4.9f), RoutePoint(350.0f, 480.0f, 4.8f),
                    RoutePoint(310.0f, 520.0f, 4.8f),  // suroeste
                    RoutePoint(350.0f, 480.0f, 4.7f), RoutePoint(390.0f, 440.0f, 4.8f),
                    RoutePoint(430.0f, 400.0f, 4.9f), RoutePoint(470.0f, 360.0f, 4.8f),
                    RoutePoint(510.0f, 320.0f, 4.8f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(510.0f, 320.0f, 225.0f, 1, 3));
            r.spawn_points.push_back(SpawnPoint(430.0f, 400.0f, 225.0f, 4, 3));
            r.spawn_points.push_back(SpawnPoint(350.0f, 480.0f, 225.0f, 5, 3));
            routes.emplace_back(r);
        }

        // RUTA 5: Circuito Perimetral (borde)
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(315.0f, 285.0f, 4.5f),  // esquina noroeste
                    RoutePoint(380.0f, 260.0f, 4.6f), RoutePoint(450.0f, 250.0f, 4.7f),
                    RoutePoint(530.0f, 270.0f, 4.6f),  // esquina noreste
                    RoutePoint(560.0f, 340.0f, 4.6f), RoutePoint(580.0f, 420.0f, 4.5f),
                    RoutePoint(560.0f, 500.0f, 4.6f),  // esquina sureste
                    RoutePoint(480.0f, 540.0f, 4.6f), RoutePoint(400.0f, 550.0f, 4.7f),
                    RoutePoint(320.0f, 540.0f, 4.6f),  // esquina suroeste
                    RoutePoint(290.0f, 480.0f, 4.6f), RoutePoint(270.0f, 400.0f, 4.5f),
                    RoutePoint(290.0f, 310.0f, 4.6f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(380.0f, 260.0f, 0.0f, 2, 4));
            r.spawn_points.push_back(SpawnPoint(560.0f, 400.0f, 270.0f, 0, 4));
            r.spawn_points.push_back(SpawnPoint(400.0f, 540.0f, 180.0f, 1, 4));
            routes.emplace_back(r);
        }

        // RUTA 6: Zig-Zag Oeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(310.0f, 310.0f, 4.2f), RoutePoint(320.0f, 360.0f, 4.2f),
                    RoutePoint(310.0f, 410.0f, 4.3f), RoutePoint(320.0f, 460.0f, 4.2f),
                    RoutePoint(310.0f, 510.0f, 4.2f), RoutePoint(330.0f, 460.0f, 4.1f),
                    RoutePoint(330.0f, 410.0f, 4.2f), RoutePoint(330.0f, 360.0f, 4.2f),
                    RoutePoint(330.0f, 310.0f, 4.1f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(315.0f, 360.0f, 90.0f, 3, 5));
            r.spawn_points.push_back(SpawnPoint(320.0f, 410.0f, 90.0f, 4, 5));
            routes.emplace_back(r);
        }

        // RUTA 7: Zig-Zag Este
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(530.0f, 310.0f, 4.2f), RoutePoint(520.0f, 360.0f, 4.2f),
                    RoutePoint(530.0f, 410.0f, 4.3f), RoutePoint(520.0f, 460.0f, 4.2f),
                    RoutePoint(530.0f, 510.0f, 4.2f), RoutePoint(510.0f, 460.0f, 4.1f),
                    RoutePoint(510.0f, 410.0f, 4.2f), RoutePoint(510.0f, 360.0f, 4.2f),
                    RoutePoint(510.0f, 310.0f, 4.1f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(525.0f, 360.0f, 270.0f, 5, 6));
            r.spawn_points.push_back(SpawnPoint(520.0f, 460.0f, 270.0f, 0, 6));
            routes.emplace_back(r);
        }

        // RUTA 8: Espiral Centro
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(410.0f, 360.0f, 4.0f), RoutePoint(450.0f, 375.0f, 4.0f),
                    RoutePoint(460.0f, 410.0f, 4.1f), RoutePoint(450.0f, 445.0f, 4.0f),
                    RoutePoint(410.0f, 460.0f, 4.0f), RoutePoint(375.0f, 445.0f, 4.1f),
                    RoutePoint(365.0f, 410.0f, 4.0f), RoutePoint(375.0f, 375.0f, 4.0f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(450.0f, 375.0f, 45.0f, 1, 7));
            r.spawn_points.push_back(SpawnPoint(410.0f, 410.0f, 180.0f, 2, 7));
            routes.emplace_back(r);
        }

        // RUTA 9: Pasillo Vertical Oeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(340.0f, 280.0f, 4.4f), RoutePoint(340.0f, 340.0f, 4.4f),
                    RoutePoint(340.0f, 400.0f, 4.5f), RoutePoint(340.0f, 460.0f, 4.4f),
                    RoutePoint(340.0f, 520.0f, 4.4f), RoutePoint(360.0f, 460.0f, 4.3f),
                    RoutePoint(360.0f, 400.0f, 4.3f), RoutePoint(360.0f, 340.0f, 4.3f),
                    RoutePoint(360.0f, 280.0f, 4.3f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(350.0f, 340.0f, 90.0f, 3, 8));
            r.spawn_points.push_back(SpawnPoint(350.0f, 460.0f, 90.0f, 4, 8));
            routes.emplace_back(r);
        }

        // RUTA 10: Pasillo Vertical Este
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(500.0f, 280.0f, 4.4f), RoutePoint(500.0f, 340.0f, 4.4f),
                    RoutePoint(500.0f, 400.0f, 4.5f), RoutePoint(500.0f, 460.0f, 4.4f),
                    RoutePoint(500.0f, 520.0f, 4.4f), RoutePoint(480.0f, 460.0f, 4.3f),
                    RoutePoint(480.0f, 400.0f, 4.3f), RoutePoint(480.0f, 340.0f, 4.3f),
                    RoutePoint(480.0f, 280.0f, 4.3f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(490.0f, 340.0f, 90.0f, 5, 9));
            r.spawn_points.push_back(SpawnPoint(490.0f, 460.0f, 90.0f, 0, 9));
            routes.emplace_back(r);
        }

        cityRoutes["liberty_city"] = routes;
    }

    void initializeViceCity() {
        std::vector<NPCRoute> routes;

        // RUTA 1: Autopista de la Playa Este-Oeste (ADAPTADA PARA TRACK VICE CITY)
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(200.0f, 48.0f, 4.8f),  // oeste
                    RoutePoint(230.0f, 45.0f, 4.8f),
                    RoutePoint(256.4f, 40.0f, 4.9f),  // cerca de CP6
                    RoutePoint(280.0f, 42.0f, 4.9f),
                    RoutePoint(300.8f, 54.0f, 5.0f),  // CP3 zona
                    RoutePoint(320.0f, 50.0f, 4.9f),
                    RoutePoint(336.5f, 45.0f, 4.9f),  // CP2 zona
                    RoutePoint(380.0f, 35.0f, 4.8f),
                    RoutePoint(420.0f, 38.0f, 4.8f),
                    RoutePoint(450.0f, 32.0f, 4.9f),
                    RoutePoint(488.9f, 30.0f, 5.0f),  // CP1 zona - ESTE
                    RoutePoint(450.0f, 35.0f, 4.8f),
                    RoutePoint(420.0f, 40.0f, 4.8f),
                    RoutePoint(380.0f, 38.0f, 4.9f),
                    RoutePoint(336.5f, 50.0f, 4.8f),
                    RoutePoint(300.8f, 52.0f, 4.9f),
                    RoutePoint(280.0f, 48.0f, 4.8f),
                    RoutePoint(256.4f, 42.0f, 4.8f),
                    RoutePoint(230.0f, 48.0f, 4.9f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(256.4f, 42.0f, 0.0f, 0, 0));
            r.spawn_points.push_back(SpawnPoint(350.0f, 45.0f, 0.0f, 1, 0));
            routes.emplace_back(r);
        }

        // RUTA 2: Autopista Norte-Sur Central
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(55.0f, 10.0f, 5.0f),  // norte
                    RoutePoint(57.0f, 30.0f, 5.0f),  RoutePoint(55.0f, 50.0f, 5.1f),
                    RoutePoint(53.0f, 70.0f, 5.0f),  RoutePoint(55.0f, 90.0f, 5.0f),
                    RoutePoint(57.0f, 110.0f, 5.0f),  // sur
                    RoutePoint(55.0f, 90.0f, 4.9f),  RoutePoint(53.0f, 70.0f, 4.9f),
                    RoutePoint(55.0f, 50.0f, 5.0f),  RoutePoint(57.0f, 30.0f, 4.9f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(55.0f, 45.0f, 90.0f, 2, 1));
            r.spawn_points.push_back(SpawnPoint(55.0f, 90.0f, 90.0f, 3, 1));
            routes.emplace_back(r);
        }

        // RUTA 3: Diagonal Noroeste-Sureste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(12.0f, 15.0f, 4.8f),  // noroeste
                    RoutePoint(28.0f, 28.0f, 4.8f),   RoutePoint(45.0f, 42.0f, 4.9f),
                    RoutePoint(62.0f, 58.0f, 5.0f),  // centro
                    RoutePoint(80.0f, 75.0f, 4.9f),   RoutePoint(100.0f, 92.0f, 4.8f),
                    RoutePoint(115.0f, 110.0f, 4.8f),  // sureste
                    RoutePoint(100.0f, 92.0f, 4.7f),  RoutePoint(80.0f, 75.0f, 4.8f),
                    RoutePoint(62.0f, 58.0f, 4.9f),   RoutePoint(45.0f, 42.0f, 4.8f),
                    RoutePoint(28.0f, 28.0f, 4.8f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(45.0f, 42.0f, 45.0f, 4, 2));
            r.spawn_points.push_back(SpawnPoint(80.0f, 75.0f, 45.0f, 5, 2));
            routes.emplace_back(r);
        }

        // RUTA 4: Diagonal Noreste-Suroeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(115.0f, 12.0f, 4.8f),  // noreste
                    RoutePoint(98.0f, 28.0f, 4.8f),  RoutePoint(80.0f, 42.0f, 4.9f),
                    RoutePoint(62.0f, 58.0f, 5.0f),  // centro
                    RoutePoint(45.0f, 72.0f, 4.9f),  RoutePoint(28.0f, 88.0f, 4.8f),
                    RoutePoint(12.0f, 105.0f, 4.8f),  // suroeste
                    RoutePoint(28.0f, 88.0f, 4.7f),  RoutePoint(45.0f, 72.0f, 4.8f),
                    RoutePoint(62.0f, 58.0f, 4.9f),  RoutePoint(80.0f, 42.0f, 4.8f),
                    RoutePoint(98.0f, 28.0f, 4.8f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(80.0f, 42.0f, 225.0f, 0, 3));
            r.spawn_points.push_back(SpawnPoint(45.0f, 72.0f, 225.0f, 1, 3));
            routes.emplace_back(r);
        }

        // RUTA 5: Circuito Perimetral Externo
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(12.0f, 12.0f, 4.5f),  // esquina noroeste
                    RoutePoint(40.0f, 8.0f, 4.6f),    RoutePoint(65.0f, 10.0f, 4.7f),
                    RoutePoint(105.0f, 15.0f, 4.6f),  // esquina noreste
                    RoutePoint(115.0f, 40.0f, 4.6f),  RoutePoint(118.0f, 70.0f, 4.5f),
                    RoutePoint(115.0f, 100.0f, 4.6f),  // esquina sureste
                    RoutePoint(80.0f, 115.0f, 4.6f),  RoutePoint(50.0f, 118.0f, 4.7f),
                    RoutePoint(18.0f, 115.0f, 4.6f),  // esquina suroeste
                    RoutePoint(8.0f, 85.0f, 4.6f),    RoutePoint(5.0f, 50.0f, 4.5f),
                    RoutePoint(8.0f, 20.0f, 4.6f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(65.0f, 10.0f, 0.0f, 2, 4));
            r.spawn_points.push_back(SpawnPoint(115.0f, 65.0f, 270.0f, 3, 4));
            routes.emplace_back(r);
        }

        // RUTA 6: Zig-Zag Oeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(18.0f, 20.0f, 4.2f),  RoutePoint(23.0f, 40.0f, 4.2f),
                    RoutePoint(18.0f, 60.0f, 4.3f),  RoutePoint(23.0f, 80.0f, 4.2f),
                    RoutePoint(18.0f, 100.0f, 4.2f), RoutePoint(28.0f, 90.0f, 4.1f),
                    RoutePoint(28.0f, 70.0f, 4.2f),  RoutePoint(28.0f, 50.0f, 4.2f),
                    RoutePoint(28.0f, 30.0f, 4.1f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(20.0f, 60.0f, 90.0f, 4, 5));
            routes.emplace_back(r);
        }

        // RUTA 7: Zig-Zag Este
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(110.0f, 20.0f, 4.2f),  RoutePoint(105.0f, 40.0f, 4.2f),
                    RoutePoint(110.0f, 60.0f, 4.3f),  RoutePoint(105.0f, 80.0f, 4.2f),
                    RoutePoint(110.0f, 100.0f, 4.2f), RoutePoint(100.0f, 90.0f, 4.1f),
                    RoutePoint(100.0f, 70.0f, 4.2f),  RoutePoint(100.0f, 50.0f, 4.2f),
                    RoutePoint(100.0f, 30.0f, 4.1f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(107.5f, 60.0f, 270.0f, 5, 6));
            routes.emplace_back(r);
        }

        // RUTA 8: Espiral Centro
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(55.0f, 30.0f, 4.0f), RoutePoint(70.0f, 35.0f, 4.0f),
                    RoutePoint(75.0f, 50.0f, 4.1f), RoutePoint(70.0f, 65.0f, 4.0f),
                    RoutePoint(55.0f, 70.0f, 4.0f), RoutePoint(40.0f, 65.0f, 4.1f),
                    RoutePoint(35.0f, 50.0f, 4.0f), RoutePoint(40.0f, 35.0f, 4.0f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(65.0f, 42.0f, 45.0f, 0, 7));
            routes.emplace_back(r);
        }

        // RUTA 9: Pasillo Vertical Oeste
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(30.0f, 10.0f, 4.4f), RoutePoint(30.0f, 30.0f, 4.4f),
                    RoutePoint(30.0f, 50.0f, 4.5f), RoutePoint(30.0f, 70.0f, 4.4f),
                    RoutePoint(30.0f, 90.0f, 4.4f), RoutePoint(30.0f, 110.0f, 4.4f),
                    RoutePoint(33.0f, 90.0f, 4.3f), RoutePoint(33.0f, 70.0f, 4.3f),
                    RoutePoint(33.0f, 50.0f, 4.3f), RoutePoint(33.0f, 30.0f, 4.3f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(30.0f, 50.0f, 90.0f, 1, 8));
            routes.emplace_back(r);
        }

        // RUTA 10: Pasillo Vertical Este
        {
            std::vector<RoutePoint> route = {
                    RoutePoint(85.0f, 10.0f, 4.4f), RoutePoint(85.0f, 30.0f, 4.4f),
                    RoutePoint(85.0f, 50.0f, 4.5f), RoutePoint(85.0f, 70.0f, 4.4f),
                    RoutePoint(85.0f, 90.0f, 4.4f), RoutePoint(85.0f, 110.0f, 4.4f),
                    RoutePoint(82.0f, 90.0f, 4.3f), RoutePoint(82.0f, 70.0f, 4.3f),
                    RoutePoint(82.0f, 50.0f, 4.3f), RoutePoint(82.0f, 30.0f, 4.3f),
            };
            NPCRoute r(route, true);
            r.spawn_points.push_back(SpawnPoint(85.0f, 50.0f, 90.0f, 2, 9));
            routes.emplace_back(r);
        }

        cityRoutes["vice_city"] = routes;
    }

public:
    NPCRouteManager() { initializeAllCities(); }

    /**
     * @brief Inicializa todas las rutas de todas las ciudades
     */
    void initializeAllCities() {
        initializeSanAndreas();
        initializeLibertyCity();
        initializeViceCity();
    }

    /**
     * @brief Obtiene las rutas para una ciudad específica
     * @param cityName Nombre de la ciudad (sin extensión): "san_andreas", "liberty_city",
     * "vice_city"
     */
    const std::vector<NPCRoute>& getRoutes(const std::string& cityName) const {
        auto it = cityRoutes.find(cityName);
        if (it != cityRoutes.end()) {
            return it->second;
        }
        // Devolver rutas vacías si no existe la ciudad
        static const std::vector<NPCRoute> emptyRoutes;
        return emptyRoutes;
    }
};

#endif  // NPC_ROUTES_H
