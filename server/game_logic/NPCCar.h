#ifndef NPC_CAR_H
#define NPC_CAR_H

#include "Car.h"
#include <box2d/box2d.h>

/**
 * @class NPCCar
 * @brief Vehículo especializado para NPCs con control de movimiento predecible
 *
 * Hereda de Car pero reemplaza la lógica de movimiento para que sea más
 * controlable y predecible para sistemas de IA. Permite control directo
 * de velocidad deseada sin aceleración/frenado complejo.
 */
class NPCCar : public Car {
private:
    float targetVelocity = 0.0f;  // Velocidad objetivo directa
    float accelerationRate = 5.0f;  // Qué tan rápido llega a velocidad objetivo (m/s²)
    
public:
    NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation);
    ~NPCCar() override = default;

    /**
     * @brief Establece velocidad objetivo directamente
     * @param velocity Velocidad deseada (0.0 = parado, > 0 = adelante, < 0 = atrás)
     */
    void setTargetVelocity(float velocity);

    /**
     * @brief Obtiene la velocidad objetivo actual
     */
    float getTargetVelocity() const { return targetVelocity; }

    /**
     * @brief Obtiene la tasa de aceleración
     */
    float getAccelerationRate() const { return accelerationRate; }

    /**
     * @brief Establece la tasa de aceleración
     * @param rate Aceleración en m/s²
     */
    void setAccelerationRate(float rate) { accelerationRate = std::max(1.0f, rate); }

    /**
     * @brief Actualiza física del NPC con control simple de velocidad
     * @param targetVel Velocidad objetivo (-1.0 a 1.0, normalizado)
     * @param turnDir Dirección de giro (FORWARD, LEFT, RIGHT)
     */
    void updateNPCPhysics(float targetVel, Direction turnDir);

    /**
     * @brief Aplica la aceleración/frenado para llegar a velocidad objetivo
     */
    void applyAcceleration(float deltaTime);

    /**
     * @brief Rota el cuerpo físico del NPC instantáneamente a una nueva dirección
     * @param direction Dirección a la que rotar (FORWARD, LEFT, RIGHT)
     */
    void rotateBodyToDirection(Direction direction);

    /**
     * @brief Rota el cuerpo físico del NPC a un ángulo específico en radianes
     * @param angleRadians Ángulo en radianes
     */
    void rotateToAngle(float angleRadians);

    /**
     * @brief Maneja el giro del NPC con rotación más agresiva que Car::handleTurning
     * Optimizado para NPCs que se quedan atascados
     * @param turnDir Dirección de giro
     * @param speed Velocidad actual
     */
    void handleTurning(Direction turnDir, float speed);
};

#endif
