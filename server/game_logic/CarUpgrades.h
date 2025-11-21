#ifndef CAR_UPGRADES_H
#define CAR_UPGRADES_H

#include <cstdint>

/**
 * @struct CarUpgrades
 * @brief Almacena los valores de mejora para cada estadística del auto.
 *
 * Cada campo representa el incremento acumulado para esa estadística.
 * El costo de aplicar una mejora se computa como una penalización de tiempo.
 */
struct CarUpgrades {
    float acceleration_boost;  // Incremento de aceleración
    float speed_boost;         // Incremento de velocidad máxima
    float handling_boost;      // Incremento de controlabilidad
    float health_boost;        // Incremento de salud máxima

    CarUpgrades():
            acceleration_boost(0.0f), speed_boost(0.0f), handling_boost(0.0f), health_boost(0.0f) {}

    /**
     * @brief Obtiene el costo total de tiempo (en segundos) de las mejoras aplicadas.
     *
     * El costo se calcula como:
     * - Aceleración: 2% del boost por cada unidad
     * - Velocidad: 1.5% del boost por cada unidad
     * - Controlabilidad: 1% del boost por cada unidad
     * - Salud: 0.5% del boost por cada unidad
     *
     * @return Penalización de tiempo en segundos
     */
    float getTimePenalty() const {
        float penalty = 0.0f;
        penalty += acceleration_boost * 0.02f;  // 2% por unidad de aceleración
        penalty += speed_boost * 0.015f;        // 1.5% por unidad de velocidad
        penalty += handling_boost * 0.01f;      // 1% por unidad de controlabilidad
        penalty += health_boost * 0.005f;       // 0.5% por unidad de salud
        return penalty;
    }

    /**
     * @brief Valida si los valores de upgrade son realizables (no negativos, no excesivos).
     * @return true si los upgrades son válidos, false en caso contrario
     */
    bool isValid() const {
        // No permitir upgrades negativos
        if (acceleration_boost < 0.0f || speed_boost < 0.0f || handling_boost < 0.0f ||
            health_boost < 0.0f) {
            return false;
        }

        // Limitar boost máximo para cada estadística (prevenir abuso)
        const float MAX_BOOST = 100.0f;
        if (acceleration_boost > MAX_BOOST || speed_boost > MAX_BOOST ||
            handling_boost > MAX_BOOST || health_boost > MAX_BOOST) {
            return false;
        }

        return true;
    }
};

#endif  // CAR_UPGRADES_H
