#include "NPCCar.h"
#include <algorithm>
#include <cmath>

NPCCar::NPCCar(b2WorldId world, const CarStats& stats, b2Vec2 position, b2Rot rotation)
        : Car(world, stats, position, rotation), targetVelocity(0.0f), accelerationRate(5.0f) {}

void NPCCar::setTargetVelocity(float velocity) {
    // Limitar a rango sensato: -1.0 (atrás) a 1.0 (adelante)
    targetVelocity = std::clamp(velocity, -1.0f, 1.0f);
}

void NPCCar::applyAcceleration(float deltaTime) {
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    
    // Determinar dirección actual (hacia adelante del auto)
    b2Vec2 forward = b2Normalize(b2RotateVector(b2Body_GetRotation(body), {1, 0}));
    float forwardSpeed = b2Dot(velocity, forward);
    
    // Velocidad máxima del auto
    float maxSpeed = getMaxSpeed();
    
    // Velocidad objetivo en unidades del mundo (simplemente targetVelocity * maxSpeed)
    float targetSpeed = targetVelocity * maxSpeed;
    
    // Acelerar/frenar gradualmente hacia velocidad objetivo
    float speedDifference = targetSpeed - forwardSpeed;
    float acceleration = std::clamp(speedDifference, -accelerationRate * deltaTime, accelerationRate * deltaTime);
    
    float newForwardSpeed = forwardSpeed + acceleration;
    
    // Aplicar nueva velocidad en la dirección forward
    b2Vec2 newVelocity = b2MulSV(newForwardSpeed, forward);
    
    b2Body_SetLinearVelocity(body, newVelocity);
}

void NPCCar::updateNPCPhysics(float targetVel, Direction turnDir) {
    // Establecer velocidad objetivo
    setTargetVelocity(targetVel);
    
    // Aplicar aceleración suave
    applyAcceleration(1.0f / 30.0f);  // Asumir 30 FPS
    
    // Aplicar giro usando la lógica existente de Car
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    float speed = b2Length(velocity);
    
    // Usar método existente de manejo de giros
    handleTurning(turnDir, speed);
}

void NPCCar::rotateBodyToDirection(Direction direction) {
    // Obtener rotación y velocidad actual
    b2Rot currentRot = b2Body_GetRotation(body);
    b2Vec2 velocity = b2Body_GetLinearVelocity(body);
    float speed = b2Length(velocity);
    
    // Si no hay movimiento suficiente, solo cambiar dirección gradualmente
    if (speed < 0.1f) {
        return;
    }
    
    // Calcular el nuevo ángulo basado en dirección
    float newAngle = b2Rot_GetAngle(currentRot);
    
    if (direction == Direction::LEFT) {
        // Rotar 90 grados a la izquierda (pi/2 radianes)
        newAngle = newAngle + (B2_PI / 2.0f);
    } else if (direction == Direction::RIGHT) {
        // Rotar 90 grados a la derecha (-pi/2 radianes)
        newAngle = newAngle - (B2_PI / 2.0f);
    } else if (direction == Direction::FORWARD) {
        // Continuar en la misma dirección (sin cambios)
        return;
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
