#ifndef NPC_CAR_H
#define NPC_CAR_H

#include <box2d/box2d.h>

#include "Car.h"
#include "SensorData.h"


class NPCCar: public Car {
private:
    bool isParked;
    bool isBlocked;
    b2Vec2 lastPos = {0, 0};
    int blockedFrames = 0;
    uint8_t carType;

    void handleBlocked();

public:
    NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked,
           uint8_t carType);
    ~NPCCar() override = default;

    void updatePhysics(const CarInput& input) override;

    void chooseIntersectionDirection(int intersectionId) override;

    bool isNPCBlocked();

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
