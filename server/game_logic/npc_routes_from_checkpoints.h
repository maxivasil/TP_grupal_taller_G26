#ifndef NPC_ROUTES_FROM_CHECKPOINTS_H
#define NPC_ROUTES_FROM_CHECKPOINTS_H

#include <algorithm>
#include <cmath>
#include <iostream>
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
 * @struct NPCRoute
 * @brief Una ruta completa que un NPC puede seguir
 */
struct NPCRoute {
    std::vector<RoutePoint> points;
    bool looping = true;  // ¿La ruta se repite?

    NPCRoute() = default;
    NPCRoute(const std::vector<RoutePoint>& pts, bool loop = true): points(pts), looping(loop) {}
};

#endif  // NPC_ROUTES_FROM_CHECKPOINTS_H
