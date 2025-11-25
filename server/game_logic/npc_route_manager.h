#ifndef NPC_ROUTE_MANAGER_H
#define NPC_ROUTE_MANAGER_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "npc_routes_from_checkpoints.h"

/**
 * @struct NPCCheckpoint
 * @brief Define un checkpoint con su posición y dimensiones para rutas de NPC
 */
struct NPCCheckpoint {
    float x;
    float y;
    float width;
    float height;
};

/**
 * @class NPCRouteManager
 * @brief Genera rutas para NPCs basándose en checkpoints
 */
class NPCRouteManager {
private:
    std::map<std::string, std::vector<NPCRoute>> cityRoutes;

public:
    NPCRouteManager() {}

    /**
     * @brief Inicializa las rutas para una ciudad desde sus checkpoints
     * @param cityName Nombre de la ciudad
     * @param checkpoints Vector de checkpoints
     */
    void initializeCityFromCheckpoints(const std::string& cityName,
                                       const std::vector<NPCCheckpoint>& checkpoints) {
        std::vector<NPCRoute> routes;

        if (checkpoints.empty()) {
            cityRoutes[cityName] = routes;
            std::cerr << "[NPC Route Manager] No checkpoints provided for " << cityName
                      << std::endl;
            return;
        }

        // Clasificar checkpoints por orientación
        std::vector<size_t> horizontalIndices;  // width > height
        std::vector<size_t> verticalIndices;    // height > width

        for (size_t i = 0; i < checkpoints.size(); ++i) {
            const auto& cp = checkpoints[i];
            if (cp.width > cp.height) {
                horizontalIndices.push_back(i);
            } else {
                verticalIndices.push_back(i);
            }
        }

        // RUTA 1: Usando checkpoints horizontales (E-W)
        if (!horizontalIndices.empty()) {
            std::vector<RoutePoint> route;
            for (size_t idx: horizontalIndices) {
                const auto& cp = checkpoints[idx];
                float leftX = cp.x - cp.width / 2.0f;
                float rightX = cp.x + cp.width / 2.0f;

                route.push_back(RoutePoint(leftX, cp.y, 5.0f));
                route.push_back(RoutePoint(cp.x, cp.y, 5.1f));
                route.push_back(RoutePoint(rightX, cp.y, 5.0f));
            }
            if (route.size() > 1) {
                for (int i = horizontalIndices.size() - 2; i >= 0; --i) {
                    const auto& cp = checkpoints[horizontalIndices[i]];
                    float leftX = cp.x - cp.width / 2.0f;
                    route.push_back(RoutePoint(leftX, cp.y, 4.9f));
                    route.push_back(RoutePoint(cp.x, cp.y, 4.9f));
                }
                routes.emplace_back(route, true);
            }
        }

        // RUTA 2: Usando checkpoints verticales (N-S)
        if (!verticalIndices.empty()) {
            std::vector<RoutePoint> route;
            for (size_t idx: verticalIndices) {
                const auto& cp = checkpoints[idx];
                float topY = cp.y - cp.height / 2.0f;
                float bottomY = cp.y + cp.height / 2.0f;

                route.push_back(RoutePoint(cp.x, topY, 5.0f));
                route.push_back(RoutePoint(cp.x, cp.y, 5.1f));
                route.push_back(RoutePoint(cp.x, bottomY, 5.0f));
            }
            if (route.size() > 1) {
                for (int i = verticalIndices.size() - 2; i >= 0; --i) {
                    const auto& cp = checkpoints[verticalIndices[i]];
                    float topY = cp.y - cp.height / 2.0f;
                    route.push_back(RoutePoint(cp.x, topY, 4.9f));
                    route.push_back(RoutePoint(cp.x, cp.y, 4.9f));
                }
                routes.emplace_back(route, true);
            }
        }

        // RUTA 3: Zigzag entre checkpoints horizontales
        if (horizontalIndices.size() > 1) {
            std::vector<RoutePoint> route;
            for (size_t i = 0; i < horizontalIndices.size(); ++i) {
                const auto& cp = checkpoints[horizontalIndices[i]];
                float offset = (i % 2 == 0) ? -30.0f : 30.0f;

                route.push_back(RoutePoint(cp.x - cp.width / 2, cp.y + offset, 4.5f));
                route.push_back(RoutePoint(cp.x + cp.width / 2, cp.y + offset, 4.5f));
            }
            if (route.size() > 1) {
                routes.emplace_back(route, true);
            }
        }

        // RUTA 4: Zigzag entre checkpoints verticales
        if (verticalIndices.size() > 1) {
            std::vector<RoutePoint> route;
            for (size_t i = 0; i < verticalIndices.size(); ++i) {
                const auto& cp = checkpoints[verticalIndices[i]];
                float offset = (i % 2 == 0) ? -30.0f : 30.0f;

                route.push_back(RoutePoint(cp.x + offset, cp.y - cp.height / 2, 4.5f));
                route.push_back(RoutePoint(cp.x + offset, cp.y + cp.height / 2, 4.5f));
            }
            if (route.size() > 1) {
                routes.emplace_back(route, true);
            }
        }

        // RUTA 5: Circuito perimetral usando todos los checkpoints
        {
            std::vector<RoutePoint> route;

            float minX = checkpoints[0].x;
            float maxX = minX;

            float minY = checkpoints[0].y;
            float maxY = minY;

            for (const auto& cp: checkpoints) {
                minX = std::min(minX, cp.x - cp.width / 2);
                maxX = std::max(maxX, cp.x + cp.width / 2);
                minY = std::min(minY, cp.y - cp.height / 2);
                maxY = std::max(maxY, cp.y + cp.height / 2);
            }

            float margin = 20.0f;
            route.push_back(RoutePoint(minX - margin, minY - margin, 4.6f));
            route.push_back(RoutePoint(maxX + margin, minY - margin, 4.6f));
            route.push_back(RoutePoint(maxX + margin, maxY + margin, 4.6f));
            route.push_back(RoutePoint(minX - margin, maxY + margin, 4.6f));

            routes.emplace_back(route, true);
        }

        // Crear rutas adicionales (hasta 10 total)
        while (routes.size() < 10 && !checkpoints.empty()) {
            std::vector<RoutePoint> route;

            int step = std::max(1, (int)checkpoints.size() / 3);
            for (size_t i = 0; i < checkpoints.size(); i += step) {
                const auto& cp = checkpoints[i];
                float offsetX = (rand() % 40) - 20;
                float offsetY = (rand() % 40) - 20;

                route.push_back(RoutePoint(cp.x + offsetX, cp.y + offsetY, 4.2f));

                if (cp.width > cp.height) {
                    route.push_back(RoutePoint(cp.x + cp.width / 3, cp.y + offsetY, 4.2f));
                    route.push_back(RoutePoint(cp.x - cp.width / 3, cp.y + offsetY, 4.2f));
                } else {
                    route.push_back(RoutePoint(cp.x + offsetX, cp.y + cp.height / 3, 4.2f));
                    route.push_back(RoutePoint(cp.x + offsetX, cp.y - cp.height / 3, 4.2f));
                }
            }

            if (route.size() > 2) {
                routes.emplace_back(route, true);
            }
        }

        cityRoutes[cityName] = routes;

        std::cout << "[NPC Route Manager] Generadas " << routes.size() << " rutas para " << cityName
                  << " desde " << checkpoints.size() << " checkpoints" << std::endl;
    }

    /**
     * @brief Obtiene las rutas para una ciudad específica
     */
    const std::vector<NPCRoute>& getRoutes(const std::string& cityName) const {
        auto it = cityRoutes.find(cityName);
        if (it != cityRoutes.end()) {
            return it->second;
        }
        static const std::vector<NPCRoute> emptyRoutes;
        return emptyRoutes;
    }
};

#endif  // NPC_ROUTE_MANAGER_H
