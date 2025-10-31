#include "city.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

#define LIBERTY_CITY_MAP_PATH "cities/liberty_city.yaml"
#define SAN_ANDREAS_MAP_PATH "cities/san_andreas.yaml"
#define VICE_CITY_MAP_PATH "cities/vice_city.yaml"

City::City(CityName name): name(name) {
    std::string filename = getYamlFileName();
    
    try {
        YAML::Node data = YAML::LoadFile(filename);

        for (const auto& obj: data["objects"]) {
            std::string type = obj["type"].as<std::string>();

            if (type == "CollisionUp" || type == "CollisionDown") {
                staticObjects.push_back({
                    obj["x"].as<float>(), 
                    obj["y"].as<float>(),
                    obj["width"].as<float>(), 
                    obj["height"].as<float>(),
                    (type == "CollisionUp")
                });
            } else if (type == "BridgeLevelSensor") {
                bridgeSensors.push_back({
                    obj["x"].as<float>(), 
                    obj["y"].as<float>(),
                    obj["width"].as<float>(), 
                    obj["height"].as<float>()
                });
            }
        }
        
        std::cout << "Loaded " << staticObjects.size() << " static objects and " 
                  << bridgeSensors.size() << " bridge sensors\n";
                  
    } catch (const YAML::Exception& e) {
        std::cerr << "Error loading city: " << e.what() << "\n";
    }
}

std::string City::getYamlFileName() const {
    switch (name) {
        case LibertyCity:
            return LIBERTY_CITY_MAP_PATH;
        case SanAndreas:
            return SAN_ANDREAS_MAP_PATH;
        case ViceCity:
            return VICE_CITY_MAP_PATH;
    }
    return "";
}

const std::vector<StaticObjectData>& City::getStaticObjects() const { 
    return staticObjects; 
}

const std::vector<BridgeSensorData>& City::getBridgeSensors() const { 
    return bridgeSensors; 
}

void City::render(SDL_Renderer* renderer, const SDL_FRect& camera) const {
    // Renderizar objetos estáticos
    for (const auto& obj : staticObjects) {
        SDL_FRect rect = {
            obj.x - camera.x,
            obj.y - camera.y,
            obj.width,
            obj.height
        };
        
        // Solo renderizar si está visible
        if (rect.x + rect.w < 0 || rect.x > 800 ||
            rect.y + rect.h < 0 || rect.y > 600) {
            continue;
        }
        
        // Color según nivel
        if (obj.isUp) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); // Gris oscuro (nivel superior)
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gris (nivel inferior)
        }
        
        SDL_RenderFillRectF(renderer, &rect);
        
        // Borde negro
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRectF(renderer, &rect);
    }
    
    // Renderizar sensores de puente (opcional, para debug)
    for (const auto& sensor : bridgeSensors) {
        SDL_FRect rect = {
            sensor.x - camera.x,
            sensor.y - camera.y,
            sensor.width,
            sensor.height
        };
        
        if (rect.x + rect.w < 0 || rect.x > 800 ||
            rect.y + rect.h < 0 || rect.y > 600) {
            continue;
        }
        
        // Amarillo semi-transparente
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
        SDL_RenderFillRectF(renderer, &rect);
    }
}

City::~City() {}
