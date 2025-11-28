#ifndef CAR_STATS_H
#define CAR_STATS_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @file CarStats.h
 * @brief Definiciones de estadísticas para cada tipo de auto disponible
 *
 * Este archivo centraliza las propiedades de cada auto para que sean consistentes
 * entre cliente y servidor.
 */

// Estructura base de estadísticas (igual que en servidor)
struct CarStats {
    float acceleration;
    float max_speed;
    float turn_speed;
    float mass;
    float brake_force;
    float handling;
    float health_max;
    float length;
    float width;
};

/**
 * @class CarStatsDatabase
 * @brief Base de datos de estadísticas de autos
 *
 * Proporciona las propiedades de cada tipo de auto disponible.
 * Los autos disponibles son:
 * - Van (Iveco Daily): Lento, resistente, poco manejable
 * - Ferrari (Dodge Viper): Muy rápido, ágil, frágil
 * - Celeste (Chevrolet Corsa): Equilibrado
 * - Jeep (Jeep Wrangler): Resistente, poco ágil
 * - Pickup (Toyota Tacoma): Resistente, pesado
 * - Limo (Lincoln TownCar): Lento, elegante
 * - Descapotable (Lamborghini Diablo): Deportivo
 */
class CarStatsDatabase {
public:
    /**
     * @brief Obtiene las estadísticas de un auto por nombre
     * @param car_name Nombre del auto (ej: "Iveco Daily")
     * @return CarStats del auto, o stats por defecto si no existe
     */
    static CarStats getCarStats(const std::string& car_name) {
        auto it = stats_map.find(car_name);
        if (it != stats_map.end()) {
            return it->second;
        }
        // Retornar stats por defecto si no encuentra
        return stats_map.at("Chevrolet Corsa");
    }

    // Función helper para convertir nombre del auto a car_type (0-6)
    static uint8_t getCarTypeFromName(const std::string& carName) {
        if (carName == "Iveco Daily")
            return 0;
        if (carName == "Dodge Viper")
            return 1;
        if (carName == "Chevrolet Corsa")
            return 2;
        if (carName == "Jeep Wrangler")
            return 3;
        if (carName == "Toyota Tacoma")
            return 4;
        if (carName == "Lincoln TownCar")
            return 5;
        if (carName == "Lamborghini Diablo")
            return 6;
        return 0;  // Default
    }

    /**
     * @brief Verifica si un nombre de auto es válido
     * @param car_name Nombre del auto
     * @return true si el auto existe en la BD
     */
    static bool isValidCarName(const std::string& car_name) {
        return stats_map.find(car_name) != stats_map.end();
    }

    /**
     * @brief Obtiene lista de todos los autos disponibles
     * @return Vector de nombres de autos
     */
    static std::vector<std::string> getAllCarNames() {
        std::vector<std::string> names;
        names.reserve(stats_map.size());

        std::transform(stats_map.begin(), stats_map.end(), std::back_inserter(names),
                       [](const auto& pair) { return pair.first; });

        return names;
    }

private:
    /**
     * @brief Mapa de estadísticas de autos
     *
     * Formato: nombre_auto -> CarStats
     *
     * Características de cada auto:
     *
     * VAN (Iveco Daily):
     *   - Propósito: Carga, transporte lento
     *   - Ventaja: Alta salud, buena masa
     *   - Desventaja: Muy lento, poca controlabilidad
     *
     * FERRARI (Dodge Viper):
     *   - Propósito: Velocidad pura
     *   - Ventaja: Máxima velocidad y aceleración
     *   - Desventaja: Baja salud, frágil
     *
     * CELESTE (Chevrolet Corsa):
     *   - Propósito: Auto equilibrado
     *   - Ventaja: Buena relación precio/rendimiento
     *   - Desventaja: Nada destacado
     *
     * JEEP (Jeep Wrangler):
     *   - Propósito: Todoterreno
     *   - Ventaja: Buena salud, controlable
     *   - Desventaja: Velocidad media
     *
     * PICKUP (Toyota Tacoma):
     *   - Propósito: Trabajo pesado
     *   - Ventaja: Alta masa, buena salud
     *   - Desventaja: Lento, poco ágil
     *
     * LIMO (Lincoln TownCar):
     *   - Propósito: Lujo y elegancia
     *   - Ventaja: Buena salud, manejable
     *   - Desventaja: Muy lento
     *
     * DESCAPOTABLE (Lamborghini Diablo):
     *   - Propósito: Deportivo de lujo
     *   - Ventaja: Muy rápido, deportivo
     *   - Desventaja: Baja salud, difícil de controlar
     */
    static const std::unordered_map<std::string, CarStats> stats_map;
};

// Inicialización de la base de datos
inline const std::unordered_map<std::string, CarStats> CarStatsDatabase::stats_map = {
        // VAN - Lento pero resistente
        {"Iveco Daily", CarStats{.acceleration = 10.0f,  // Baja aceleración
                                 .max_speed = 80.0f,     // Velocidad muy baja
                                 .turn_speed = 4.0f,     // Difícil de girar
                                 .mass = 2000.0f,        // Muy pesado
                                 .brake_force = 10.0f,   // Frenos débiles
                                 .handling = 1.5f,       // Muy poco manejable
                                 .health_max = 150.0f,   // Mucha salud
                                 .length = 2.8260833f,
                                 .width = 4.019355f}},

        // FERRARI - Muy rápido pero frágil
        {"Dodge Viper", CarStats{.acceleration = 35.0f,  // Aceleración máxima
                                 .max_speed = 150.0f,    // Velocidad máxima
                                 .turn_speed = 8.5f,     // Muy ágil
                                 .mass = 800.0f,         // Muy ligero
                                 .brake_force = 18.0f,   // Frenos deportivos
                                 .handling = 4.5f,       // Muy manejable
                                 .health_max = 60.0f,    // Muy frágil
                                 .length = 2.8260833f,
                                 .width = 5.741635f}},

        // CELESTE - Equilibrado
        {"Chevrolet Corsa", CarStats{.acceleration = 20.0f,  // Aceleración normal
                                     .max_speed = 120.0f,    // Velocidad normal
                                     .turn_speed = 7.0f,     // Normal
                                     .mass = 1200.0f,        // Peso normal
                                     .brake_force = 15.0f,   // Frenos normales
                                     .handling = 2.8f,       // Manejabilidad normal
                                     .health_max = 100.0f,   // Salud normal
                                     .length = 3.083f,
                                     .width = 5.598387f}},

        // JEEP - Resistente y manejable
        {"Jeep Wrangler", CarStats{.acceleration = 18.0f,  // Aceleración buena
                                   .max_speed = 110.0f,    // Velocidad buena
                                   .turn_speed = 8.0f,     // Muy ágil
                                   .mass = 1600.0f,        // Pesado
                                   .brake_force = 16.0f,   // Frenos buenos
                                   .handling = 3.5f,       // Muy manejable
                                   .health_max = 130.0f,   // Salud alta
                                   .length = 3.083f,
                                   .width = 5.454869f}},

        // PICKUP - Peso y resistencia
        {"Toyota Tacoma", CarStats{.acceleration = 12.0f,  // Aceleración baja
                                   .max_speed = 100.0f,    // Velocidad baja
                                   .turn_speed = 5.5f,     // Poco ágil
                                   .mass = 1900.0f,        // Muy pesado
                                   .brake_force = 12.0f,   // Frenos débiles
                                   .handling = 2.0f,       // Poco manejable
                                   .health_max = 140.0f,   // Salud muy alta
                                   .length = 2.8260833f,
                                   .width = 5.741635f}},

        // LIMO - Lento pero elegante
        {"Lincoln TownCar", CarStats{.acceleration = 14.0f,  // Aceleración baja
                                     .max_speed = 90.0f,     // Velocidad baja
                                     .turn_speed = 6.5f,     // Poco ágil
                                     .mass = 1700.0f,        // Pesado
                                     .brake_force = 14.0f,   // Frenos normales
                                     .handling = 2.5f,       // Manejabilidad baja
                                     .health_max = 120.0f,   // Salud alta
                                     .length = 2.569167f,    // Muy largo
                                     .width = 6.890323f}},

        // DESCAPOTABLE - Deportivo
        {"Lamborghini Diablo", CarStats{.acceleration = 32.0f,  // Aceleración muy alta
                                        .max_speed = 145.0f,    // Velocidad muy alta
                                        .turn_speed = 9.0f,     // Muy ágil
                                        .mass = 850.0f,         // Muy ligero
                                        .brake_force = 17.0f,   // Frenos deportivos
                                        .handling = 4.2f,       // Muy manejable
                                        .health_max = 75.0f,    // Salud media-baja
                                        .length = 2.8260833f,
                                        .width = 5.741635f}}};

#endif  // CAR_STATS_H
