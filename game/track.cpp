#include "track.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <cmath>

Track::Track(const std::string& mapName) : startX(0), startY(0) {
    std::string filename = "cities/" + mapName + ".yaml";
    
    try {
        YAML::Node data = YAML::LoadFile(filename);
        name = mapName;
        
        // Cargar posición inicial
        if (data["start"]) {
            startX = data["start"]["x"].as<float>();
            startY = data["start"]["y"].as<float>();
        } else {
            // Default: centro del mapa
            startX = 2500.0f;
            startY = 2500.0f;
        }
        
        // Cargar checkpoints
        if (data["checkpoints"]) {
            int id = 0;
            for (const auto& cp : data["checkpoints"]) {
                checkpoints.push_back({
                    cp["x"].as<float>(),
                    cp["y"].as<float>(),
                    cp["width"].as<float>(),
                    cp["height"].as<float>(),
                    id,
                    cp["isFinish"].as<bool>(false)
                });
                id++;
            }
        }
        
        std::cout << "✓ Loaded track: " << mapName << " with " 
                  << checkpoints.size() << " checkpoints\n";
        std::cout << "  Start position: (" << startX << ", " << startY << ")\n";
                  
    } catch (const YAML::Exception& e) {
        std::cerr << "✗ Error loading track: " << e.what() << "\n";
        // Default track si falla
        startX = 2500.0f;
        startY = 2500.0f;
    }
}

bool Track::getStartPosition(float& x, float& y) const {
    x = startX;
    y = startY;
    return true;
}

// AABB (Axis-Aligned Bounding Box) collision detection
int Track::checkpointAtPosition(float carX, float carY, float carWidth, float carHeight) const {
    for (const auto& cp : checkpoints) {
        // Límites del auto
        float carLeft = carX - carWidth / 2;
        float carRight = carX + carWidth / 2;
        float carTop = carY - carHeight / 2;
        float carBottom = carY + carHeight / 2;
        
        // Límites del checkpoint
        float cpLeft = cp.x;
        float cpRight = cp.x + cp.width;
        float cpTop = cp.y;
        float cpBottom = cp.y + cp.height;
        
        // Verificar AABB collision
        if (carRight > cpLeft && carLeft < cpRight &&
            carBottom > cpTop && carTop < cpBottom) {
            return cp.id;  // Retorna ID del checkpoint
        }
    }
    return -1;  // No hay checkpoint
}

bool Track::isCheckpointFinish(int checkpointId) const {
    for (const auto& cp : checkpoints) {
        if (cp.id == checkpointId) {
            return cp.isFinish;
        }
    }
    return false;
}

void Track::render(SDL_Renderer* renderer, const SDL_FRect& camera) const {
    for (const auto& cp : checkpoints) {
        SDL_FRect rect = {
            cp.x - camera.x,
            cp.y - camera.y,
            cp.width,
            cp.height
        };
        
        // Solo renderizar si está visible
        if (rect.x + rect.w < 0 || rect.x > 800 ||
            rect.y + rect.h < 0 || rect.y > 600) {
            continue;
        }
        
        if (cp.isFinish) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);  // Rojo = meta
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);  // Verde = checkpoint
        }
        
        SDL_RenderFillRectF(renderer, &rect);
        
        // Borde blanco
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        SDL_RenderDrawRectF(renderer, &rect);
    }
}
