#include "NPCCar.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#define MINMOVEMENT 0.1f
#define MAXFRAMESBLOQUED 60

NPCCar::NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation, bool parked,
               uint8_t carType):
        Car(world, stats, position, rotation),
        isParked(parked),
        isBlocked(false),
        carType(carType) {}

void NPCCar::updatePhysics(const CarInput& input) {
    b2Vec2 forward = b2Normalize(b2RotateVector(b2Body_GetRotation(body), {1, 0}));
    forward = b2MulSV(getMaxSpeed(), forward);
    b2Body_SetLinearVelocity(body, forward);

    if (!isParked) {
        handleBlocked();
    }
}

void NPCCar::handleBlocked() {
    b2Vec2 currentPos = b2Body_GetPosition(body);
    float distMoved = b2Distance(currentPos, lastPos);

    if (distMoved < MINMOVEMENT) {
        blockedFrames++;

        if (blockedFrames >= MAXFRAMESBLOQUED) {
            isBlocked = true;
        }
    } else {
        blockedFrames = 0;
        isBlocked = false;
    }

    lastPos = currentPos;
}

void NPCCar::chooseIntersectionDirection(int intersectionId) {
    std::vector<float> validAngles;

    if (intersectionId & DIR_RIGHT) {
        validAngles.push_back(0.0f);
    }
    if (intersectionId & DIR_LEFT) {
        validAngles.push_back(B2_PI);
    }
    
    // Normalizar el ángulo a rango [-pi, pi]
    while (newAngle > B2_PI) newAngle -= 2.0f * B2_PI;
    while (newAngle < -B2_PI) newAngle += 2.0f * B2_PI;
    
    // Crear nueva rotación
    b2Rot newRot = b2MakeRot(newAngle);
    
    // Reorientar velocidad hacia nueva dirección forward
    b2Vec2 newForward = b2RotateVector(newRot, {1, 0});
    b2Vec2 newVelocity = b2MulSV(speed, newForward);
    
    // Aplicar nueva velocidad con nueva orientación usando torque
    // Primero cambiar velocidad hacia nueva dirección
    b2Body_SetLinearVelocity(body, newVelocity);
    
    // Aplicar torque suficiente para rotar el cuerpo físico
    float rotationDifference = newAngle - b2Rot_GetAngle(currentRot);
    float torque = rotationDifference > 0 ? 50000.0f : -50000.0f;  // Rotación rápida
    b2Body_ApplyTorque(body, torque, true);
}

void NPCCar::rotateToAngle(float angleRadians) {
    // Obtener estado actual
    b2Rot currentRot = b2Body_GetRotation(body);
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    float speed = b2Length(velocity);

    // Normalizar el ángulo a rango [-pi, pi]
    while (angleRadians > B2_PI) angleRadians -= 2.0f * B2_PI;
    while (angleRadians < -B2_PI) angleRadians += 2.0f * B2_PI;

    float currentAngle = b2Rot_GetAngle(currentRot);
    
    // Normalizar ángulo actual también
    while (currentAngle > B2_PI) currentAngle -= 2.0f * B2_PI;
    while (currentAngle < -B2_PI) currentAngle += 2.0f * B2_PI;

    // Calcular diferencia de rotación (tomar la ruta más corta)
    float rotationDifference = angleRadians - currentAngle;
    
    // Normalizar la diferencia a [-π, π]
    while (rotationDifference > B2_PI) rotationDifference -= 2.0f * B2_PI;
    while (rotationDifference < -B2_PI) rotationDifference += 2.0f * B2_PI;

    // Si ya está en el ángulo correcto (< 0.05 radianes), parar de rotar
    if (std::abs(rotationDifference) < 0.05f) {
        // Ya rotó lo suficiente, no aplicar torque
        // Detener rotación angular
        b2Body_SetAngularVelocity(body, 0.0f);
        return;
    }

    // Reorientar velocidad hacia nueva dirección forward
    b2Rot newRot = b2MakeRot(angleRadians);
    b2Vec2 newForward = b2RotateVector(newRot, {1, 0});
    b2Vec2 newVelocity = (speed > 0.01f) ? b2MulSV(speed, newForward) : b2Vec2{0, 0};

    // Aplicar nueva velocidad
    b2Body_SetLinearVelocity(body, newVelocity);

    // Aplicar torque proporcional a la diferencia (torque más suave)
    // Cuanto mayor sea la diferencia, mayor el torque, pero con límite más bajo
    float torqueStrength = std::clamp(std::abs(rotationDifference) * 8000.0f, 3000.0f, 15000.0f);
    float torque = rotationDifference > 0 ? torqueStrength : -torqueStrength;
    b2Body_ApplyTorque(body, torque, true);
}

void NPCCar::handleTurning(Direction turnDir, float speed) {
    // Los NPCs manejan su propia rotación a través de rotateToAngle()
    // No necesitan giros adicionales aquí
    // Esta función está vacía para los NPCs
}
    if (intersectionId & DIR_UP) {
        validAngles.push_back(-B2_PI / 2.0f);
    }
    if (intersectionId & DIR_DOWN) {
        validAngles.push_back(B2_PI / 2.0f);
    }

    if (validAngles.empty())
        return;

    int choice = std::rand() % validAngles.size();
    float newAngle = validAngles[choice];

    b2Vec2 currentPos = b2Body_GetPosition(body);
    b2Body_SetTransform(body, currentPos, b2MakeRot(newAngle));
}

bool NPCCar::isNPCBlocked() { return isBlocked; }

uint8_t NPCCar::getCarType() const { return carType; }
