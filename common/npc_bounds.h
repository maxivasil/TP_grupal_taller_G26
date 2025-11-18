#ifndef NPC_BOUNDS_H
#define NPC_BOUNDS_H

#include <string>
#include <map>

/**
 * @struct NPCBounds
 * @brief Define los límites del área donde pueden moverse los NPCs en cada ciudad
 */
struct NPCBounds {
    float min_x = 50.0f;
    float max_x = 650.0f;
    float min_y = 50.0f;
    float max_y = 600.0f;
    
    float getWidth() const { return max_x - min_x; }
    float getHeight() const { return max_y - min_y; }
    float getCenterX() const { return (min_x + max_x) / 2.0f; }
    float getCenterY() const { return (min_y + max_y) / 2.0f; }
};

/**
 * @class NPCBoundsManager
 * @brief Gestiona los bounds de NPCs para cada ciudad
 */
class NPCBoundsManager {
private:
    static std::map<std::string, NPCBounds> boundsMap;
    
    static void initializeBounds() {
        // San Andreas: 665.0 x 600.7 (use 50 unit margin)
        boundsMap["san_andreas"] = NPCBounds{50.0f, 650.0f, 50.0f, 600.0f};
        
        // Liberty City: 665.4 x 601.0 (use 50 unit margin)
        boundsMap["liberty_city"] = NPCBounds{50.0f, 650.0f, 50.0f, 600.0f};
        
        // Vice City: 662.8 x 600.8 (use 50 unit margin)
        boundsMap["vice_city"] = NPCBounds{50.0f, 650.0f, 50.0f, 600.0f};
    }
    
public:
    static const NPCBounds& getBounds(const std::string& cityName) {
        if (boundsMap.empty()) {
            initializeBounds();
        }
        
        auto it = boundsMap.find(cityName);
        if (it != boundsMap.end()) {
            return it->second;
        }
        
        // Default bounds if city not found
        static const NPCBounds defaultBounds{50.0f, 650.0f, 50.0f, 600.0f};
        return defaultBounds;
    }
};

// Initialize static member
std::map<std::string, NPCBounds> NPCBoundsManager::boundsMap;

#endif // NPC_BOUNDS_H
