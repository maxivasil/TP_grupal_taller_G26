#include "minimap.h"
#include <iostream>
#include <algorithm>

// Colores más BRILLANTES y SATURADOS para mejor visibilidad
const uint8_t COLOR_WATER_R = 50, COLOR_WATER_G = 200, COLOR_WATER_B = 255;      // Cian brillante
const uint8_t COLOR_BUILDING_R = 255, COLOR_BUILDING_G = 140, COLOR_BUILDING_B = 0;  // Naranja brillante
const uint8_t COLOR_CHECKPOINT_R = 100, COLOR_CHECKPOINT_G = 255, COLOR_CHECKPOINT_B = 100;  // Verde brillante
const uint8_t COLOR_FINISH_R = 255, COLOR_FINISH_G = 50, COLOR_FINISH_B = 50;    // Rojo brillante
const uint8_t COLOR_PLAYER_R = 255, COLOR_PLAYER_G = 255, COLOR_PLAYER_B = 255;  // Blanco (sin cambios)
const uint8_t COLOR_OTHER_R = 100, COLOR_OTHER_G = 200, COLOR_OTHER_B = 255;     // Azul brillante

Minimap::Minimap(int size) : size(size) {}

Minimap::~Minimap() = default;

void Minimap::loadCityData(const std::string& yamlPath) {
    try {
        YAML::Node config = YAML::LoadFile(yamlPath);
        if (!config["objects"]) {
            std::cerr << "No 'objects' section\n";
            return;
        }
        buildings.clear();
        for (const auto& obj : config["objects"]) {
            float x = obj["x"].as<float>();
            float y = obj["y"].as<float>();
            float width = obj["width"].as<float>();
            float height = obj["height"].as<float>();
            std::string type = obj["type"].as<std::string>("building");
            bool isWater = (type.find("Water") != std::string::npos);
            buildings.emplace_back();
            buildings.back().x = x;
            buildings.back().y = y;
            buildings.back().width = width;
            buildings.back().height = height;
            buildings.back().isWater = isWater;
        }
        std::cout << "Loaded " << buildings.size() << " buildings\n";
    } catch (const std::exception& e) {
        std::cerr << "Error loading city: " << e.what() << "\n";
    }
}

void Minimap::setCheckpoints(const std::vector<RaceCheckpoint>& cp) {
    checkpoints = cp;
}

float Minimap::worldToMinimapX(float worldX, float playerX) const {
    // Calcular escala usando la región zoomada, no todo el mundo
    float scale = static_cast<float>(size) / zoomWidth;
    
    // Convertir coordenada mundial a coordenada relativa de minimap
    float relativeX = worldX - playerX;
    float minimapX = (static_cast<float>(size) / 2.0f) + (relativeX * scale);
    
    return minimapX;
}

float Minimap::worldToMinimapY(float worldY, float playerY) const {
    // Calcular escala usando la región zoomada, no todo el mundo
    float scale = static_cast<float>(size) / zoomHeight;
    
    // Convertir coordenada mundial a coordenada relativa de minimap
    float relativeY = worldY - playerY;
    float minimapY = (static_cast<float>(size) / 2.0f) + (relativeY * scale);
    
    return minimapY;
}

void Minimap::renderBuilding(SDL2pp::Renderer& renderer, const Building& b,
                            float playerX, float playerY) {
    int screenX = 640 - size - 10;
    int screenY = 10;
    
    float minimapX = worldToMinimapX(b.x, playerX);
    float minimapY = worldToMinimapY(b.y, playerY);
    
    float scaleX = static_cast<float>(size) / zoomWidth;
    float scaleY = static_cast<float>(size) / zoomHeight;
    int minimapWidth = std::max(1, static_cast<int>(b.width * scaleX));
    int minimapHeight = std::max(1, static_cast<int>(b.height * scaleY));
    
    int screenMinimapX = screenX + static_cast<int>(minimapX);
    int screenMinimapY = screenY + static_cast<int>(minimapY);
    
    if (screenMinimapX < screenX - 100 || screenMinimapX > screenX + size + 100 ||
        screenMinimapY < screenY - 100 || screenMinimapY > screenY + size + 100) {
        return;
    }
    
    if (b.isWater) {
        renderer.SetDrawColor(COLOR_WATER_R, COLOR_WATER_G, COLOR_WATER_B, 255);
    } else {
        renderer.SetDrawColor(COLOR_BUILDING_R, COLOR_BUILDING_G, COLOR_BUILDING_B, 255);
    }
    
    SDL_Rect rect{ screenMinimapX, screenMinimapY, minimapWidth, minimapHeight };
    renderer.FillRect(rect);
}

void Minimap::renderCheckpoint(SDL2pp::Renderer& renderer, const RaceCheckpoint& cp,
                              float playerX, float playerY) {
    int screenX = 640 - size - 10;
    int screenY = 10;
    
    float minimapX = worldToMinimapX(cp.x, playerX);
    float minimapY = worldToMinimapY(cp.y, playerY);
    
    float scaleX = static_cast<float>(size) / zoomWidth;
    float scaleY = static_cast<float>(size) / zoomHeight;
    int minimapWidth = std::max(2, static_cast<int>(cp.width * scaleX));
    int minimapHeight = std::max(2, static_cast<int>(cp.height * scaleY));
    
    int screenMinimapX = screenX + static_cast<int>(minimapX);
    int screenMinimapY = screenY + static_cast<int>(minimapY);
    
    if (screenMinimapX < screenX - 100 || screenMinimapX > screenX + size + 100 ||
        screenMinimapY < screenY - 100 || screenMinimapY > screenY + size + 100) {
        return;
    }
    
    if (cp.isFinish) {
        renderer.SetDrawColor(COLOR_FINISH_R, COLOR_FINISH_G, COLOR_FINISH_B, 255);
    } else {
        renderer.SetDrawColor(COLOR_CHECKPOINT_R, COLOR_CHECKPOINT_G, COLOR_CHECKPOINT_B, 255);
    }
    
    SDL_Rect rect{ screenMinimapX, screenMinimapY, minimapWidth, minimapHeight };
    renderer.FillRect(rect);
}

void Minimap::renderPlayer(SDL2pp::Renderer& renderer, const MinimapPlayer& p,
                          float playerX, float playerY, bool isLocal) {
    int screenX = 640 - size - 10;
    int screenY = 10;
    
    float minimapX = worldToMinimapX(p.x, playerX);
    float minimapY = worldToMinimapY(p.y, playerY);
    
    // Convertir a pantalla
    int screenMinimapX = screenX + static_cast<int>(minimapX);
    int screenMinimapY = screenY + static_cast<int>(minimapY);
    
    if (screenMinimapX < screenX - 50 || screenMinimapX > screenX + size + 50 ||
        screenMinimapY < screenY - 50 || screenMinimapY > screenY + size + 50) {
        return;
    }
    
    if (isLocal) {
        // Jugador local - triángulo blanco que ROTA según dirección
        renderer.SetDrawColor(COLOR_PLAYER_R, COLOR_PLAYER_G, COLOR_PLAYER_B, 255);
        int radius = 6;
        
        // Calcular rotación según ángulo del jugador
        float angleRad = p.angle * 3.14159265f / 180.0f;
        float cos_a = std::cos(angleRad);
        float sin_a = std::sin(angleRad);
        
        // Triángulo rotado: apunta en la dirección del ángulo
        SDL2pp::Point points[3] = {
            // Punta del triángulo (frente del auto)
            SDL2pp::Point(static_cast<int>(screenMinimapX + radius * cos_a),
                        static_cast<int>(screenMinimapY + radius * sin_a)),
            // Esquina izquierda trasera
            SDL2pp::Point(static_cast<int>(screenMinimapX - radius * sin_a - radius * cos_a * 0.5f),
                        static_cast<int>(screenMinimapY + radius * cos_a - radius * sin_a * 0.5f)),
            // Esquina derecha trasera
            SDL2pp::Point(static_cast<int>(screenMinimapX + radius * sin_a - radius * cos_a * 0.5f),
                        static_cast<int>(screenMinimapY - radius * cos_a - radius * sin_a * 0.5f))
        };
        
        renderer.DrawLines(points, 3);
        renderer.DrawLine(points[2], points[0]);
        
        // Punto rojo en el centro para marcar la posición exacta
        renderer.SetDrawColor(255, 0, 0, 255);
        for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
                renderer.DrawPoint(screenMinimapX + dx, screenMinimapY + dy);
            }
        }
    } else {
        // Otros jugadores - triángulo azul con dirección
        renderer.SetDrawColor(COLOR_OTHER_R, COLOR_OTHER_G, COLOR_OTHER_B, 255);
        int radius = 4;
        float angleRad = p.angle * 3.14159265f / 180.0f;
        float cos_a = std::cos(angleRad);
        float sin_a = std::sin(angleRad);
        
        SDL2pp::Point points[3] = {
            SDL2pp::Point(static_cast<int>(screenMinimapX + radius * cos_a),
                        static_cast<int>(screenMinimapY + radius * sin_a)),
            SDL2pp::Point(static_cast<int>(screenMinimapX - radius * sin_a - radius * cos_a),
                        static_cast<int>(screenMinimapY + radius * cos_a - radius * sin_a)),
            SDL2pp::Point(static_cast<int>(screenMinimapX + radius * sin_a - radius * cos_a),
                        static_cast<int>(screenMinimapY - radius * cos_a - radius * sin_a))
        };
        renderer.DrawLines(points, 3);
        renderer.DrawLine(points[2], points[0]);
        
        renderer.SetDrawColor(COLOR_OTHER_R, COLOR_OTHER_G, COLOR_OTHER_B, 200);
        renderer.DrawPoint(screenMinimapX, screenMinimapY);
    }
}

void Minimap::render(SDL2pp::Renderer& renderer,
                    const MinimapPlayer& localPlayer,
                    const std::vector<MinimapPlayer>& otherPlayers) {
    // Posición: esquina superior derecha (pequeño, sin tapar el juego)
    int screenX = 640 - size - 10;
    int screenY = 10;
    
    // Fondo del minimap - gris oscuro en lugar de negro puro para mejor contraste
    renderer.SetDrawColor(20, 20, 30, 255);
    SDL_Rect minimapBg{ screenX - 2, screenY - 2, size + 4, size + 4 };
    renderer.FillRect(minimapBg);
    
    // Borde exterior - CIAN BRILLANTE para máxima visibilidad (más visible que amarillo)
    renderer.SetDrawColor(0, 255, 255, 255);
    for (int i = 0; i < 4; i++) {
        SDL_Rect border{ screenX - 2 - i, screenY - 2 - i, size + 4 + (i*2), size + 4 + (i*2) };
        renderer.DrawRect(border);
    }
    
    // Dibujar todos los edificios
    for (const auto& building : buildings) {
        renderBuilding(renderer, building, localPlayer.x, localPlayer.y);
    }
    
    // Dibujar todos los checkpoints
    for (const auto& checkpoint : checkpoints) {
        renderCheckpoint(renderer, checkpoint, localPlayer.x, localPlayer.y);
    }
    
    // Dibujar otros jugadores
    for (const auto& player : otherPlayers) {
        renderPlayer(renderer, player, localPlayer.x, localPlayer.y, false);
    }
    
    // Dibujar jugador local en el centro
    renderPlayer(renderer, localPlayer, localPlayer.x, localPlayer.y, true);
    
    // Grid de referencia
    renderer.SetDrawColor(100, 100, 100, 100);
    for (int i = 0; i <= 4; i++) {
        int x = screenX + (size / 4) * i;
        renderer.DrawLine(x, screenY, x, screenY + size);
        int y = screenY + (size / 4) * i;
        renderer.DrawLine(screenX, y, screenX + size, y);
    }
    
    std::cout << "Minimap: Player at (" << localPlayer.x << ", " << localPlayer.y 
              << "), Buildings: " << buildings.size() << "\n";
}
