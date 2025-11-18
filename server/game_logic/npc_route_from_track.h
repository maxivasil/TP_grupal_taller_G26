#ifndef NPC_ROUTE_FROM_TRACK_H
#define NPC_ROUTE_FROM_TRACK_H

#include <vector>
#include <cmath>
#include "npc_routes_from_checkpoints.h"

/**
 * @class NPCRouteFromTrack
 * @brief Genera rutas de NPCs inspiradas en los checkpoints del track
 * 
 * La idea es usar los checkpoints del track de cada carrera como base
 * para generar rutas de NPCs que pasen por esas mismas zonas, evitando
 * congestión y creando tráfico realista.
 */
class NPCRouteFromTrack {
public:
    /**
     * @brief Genera rutas de NPCs basadas en checkpoints del track
     * @param trackCheckpoints Vector de checkpoints del track
     * @param numRoutes Número de rutas a generar (default: 4-6 para menos congestión)
     * @return Vector de rutas generadas
     */
    static std::vector<NPCRoute> generateRoutesFromTrackCheckpoints(
        const std::vector<RoutePoint>& trackCheckpoints,
        int numRoutes = 5) {
        
        std::vector<NPCRoute> routes;
        
        if (trackCheckpoints.size() < 2) {
            return routes;
        }
        
        // Generar N rutas con variaciones del track principal
        for (int routeIdx = 0; routeIdx < numRoutes; ++routeIdx) {
            std::vector<RoutePoint> route = generateVariationOfTrack(
                trackCheckpoints, 
                routeIdx, 
                numRoutes
            );
            
            if (!route.empty()) {
                routes.emplace_back(route, true);  // looping = true
            }
        }
        
        return routes;
    }
    
    /**
     * @brief Genera una variación del track principal con offset perpendicular
     * @param originalTrack Ruta original del track
     * @param routeIndex Índice de esta ruta (0, 1, 2, ...)
     * @param totalRoutes Número total de rutas
     * @return Vector de puntos con variación aplicada
     */
    static std::vector<RoutePoint> generateVariationOfTrack(
        const std::vector<RoutePoint>& originalTrack,
        int routeIndex,
        int totalRoutes) {
        
        std::vector<RoutePoint> variation;
        
        if (originalTrack.size() < 2) return variation;
        
        // Calcular offset perpendicular basado en el índice de ruta
        // Distribuir rutas a los lados del track original
        float offsetDistance = (routeIndex - totalRoutes / 2.0f) * 2.5f;  // ±2.5 por ruta
        
        // Copiar el track con offset perpendicular
        for (size_t i = 0; i < originalTrack.size(); ++i) {
            const auto& current = originalTrack[i];
            
            // Calcular vector perpendicular para el offset
            b2Vec2 direction;
            if (i < originalTrack.size() - 1) {
                const auto& next = originalTrack[i + 1];
                direction = {next.x - current.x, next.y - current.y};
            } else if (i > 0) {
                const auto& prev = originalTrack[i - 1];
                direction = {current.x - prev.x, current.y - prev.y};
            } else {
                direction = {1, 0};
            }
            
            // Normalizar
            float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (len > 0.001f) {
                direction.x /= len;
                direction.y /= len;
            }
            
            // Vector perpendicular (rotar 90 grados)
            float perpX = -direction.y;
            float perpY = direction.x;
            
            // Aplicar offset
            float offsetX = perpX * offsetDistance;
            float offsetY = perpY * offsetDistance;
            
            // Añadir pequeña variación aleatoria longitudinal en rutas alternas
            float longitudinalVar = 0.0f;
            if (routeIndex % 2 == 1) {
                // Rutas alternas tienen pequeña desviación longitudinal
                longitudinalVar = 1.5f * std::sin(i * 0.5f);
            }
            
            variation.push_back(RoutePoint(
                current.x + offsetX + direction.x * longitudinalVar,
                current.y + offsetY + direction.y * longitudinalVar,
                current.speed
            ));
        }
        
        return variation;
    }
    
    /**
     * @brief Convierte checkpoints de track format a RoutePoint format
     */
    // Este método no se necesita, eliminado para evitar dependencias circulares
};

#endif // NPC_ROUTE_FROM_TRACK_H
