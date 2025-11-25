#include "NPCTraffic.h"

#include <cmath>
#include <cstdlib>

// Static random direction counter for variety
static int randomDirectionCounter = 0;

b2BodyDef NPCTraffic::initNPCBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.userData = this;
    bodyDef.isAwake = true;
    bodyDef.linearDamping = 0.7f;  // Damping similar a los autos jugadores
    bodyDef.angularDamping = 1.0f;
    return bodyDef;
}

void NPCTraffic::setShape(b2BodyId body) {
    // Crear forma rectangular aproximada del auto (0.4m x 0.2m = 40cm x 20cm)
    // Similar a los autos del jugador
    b2Polygon polygon = b2MakeBox(0.2f, 0.1f);

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.0f;

    b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &polygon);

    b2Body_ApplyMassFromShapes(body);
}

NPCTraffic::NPCTraffic(b2WorldId world, uint8_t carType_, b2Vec2 position):
        carType(carType_), current_health(100.0f), maxHealth(100.0f),
        currentDirection(rand() % 4), lastPosition(position), stuckTimer(0.0f),
        collisionCount(0), isParked(false) {

    b2BodyDef bodyDef = initNPCBodyDef(position);
    body = b2CreateBody(world, &bodyDef);

    setShape(body);
}

NPCTraffic::~NPCTraffic() {
    // El body se destruye cuando se destruye el world
}

void NPCTraffic::updatePhysics(float deltaTime, const std::vector<RoutePoint>* route) {
    if (isDestroyed()) {
        return;
    }

    // Si está estacionado, no moverse
    if (isParked) {
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
        b2Body_SetAngularVelocity(body, 0.0f);
        return;
    }

    // Obtener posición actual
    b2Vec2 currentPos = b2Body_GetPosition(body);

    // ===== DETECCIÓN DE ESQUINAS (basada en colisiones/obstáculos) =====
    // Detectar si el NPC está estancado (velocidad cercana a 0)
    b2Vec2 currentVel = b2Body_GetLinearVelocity(body);
    float currentSpeed = b2Length(currentVel);
    float distMoved = b2Distance(lastPosition, currentPos);

    stuckTimer += deltaTime;

    // Si el NPC se movió muy poco en este frame Y se intenta mover
    // Significa que hay un obstáculo -> decidir cambiar dirección
    if (distMoved < 0.05f && stuckTimer > 0.3f) {
        // Hay un obstáculo o esquina, cambiar dirección
        stuckTimer = 0.0f;

        // 40% cambiar a una dirección aleatoria
        // 60% intentar girar 90° (esquina natural)
        if ((rand() % 100) < 40) {
            currentDirection = rand() % 4;
        } else {
            // Girar 90° (izquierda o derecha)
            currentDirection = (currentDirection + (rand() % 2 == 0 ? 1 : 3)) % 4;
        }
    }

    // Resetear stuck timer si se está moviendo bien
    if (distMoved > 0.1f) {
        stuckTimer = 0.0f;
    }

    // Recordar posición para próximo frame
    lastPosition = currentPos;

    // Velocidad base (metros/segundo)
    targetSpeed = 4.0f + (rand() % 10) / 50.0f;  // Entre 4.0 y 4.2 m/s

    // Vector de dirección según currentDirection
    b2Vec2 direction = {0.0f, 0.0f};
    float targetAngle = 0.0f;

    switch (currentDirection % 4) {
        case 0:  // Norte (arriba)
            direction = {0.0f, -1.0f};
            targetAngle = 1.5708f;  // π/2 radianes
            break;
        case 1:  // Este (derecha)
            direction = {1.0f, 0.0f};
            targetAngle = 0.0f;
            break;
        case 2:  // Sur (abajo)
            direction = {0.0f, 1.0f};
            targetAngle = -1.5708f;  // -π/2 radianes
            break;
        case 3:  // Oeste (izquierda)
            direction = {-1.0f, 0.0f};
            targetAngle = 3.14159f;  // π radianes
            break;
    }

    // Aplicar velocidad en dirección de movimiento
    b2Vec2 desiredVelocity = b2MulSV(targetSpeed, direction);

    // Suavizar cambios de velocidad
    b2Vec2 velocityChange = b2Sub(desiredVelocity, currentVel);
    velocityChange = b2MulSV(0.5f, velocityChange);

    b2Vec2 newVelocity = b2Add(currentVel, velocityChange);

    // Limitar a velocidad máxima
    float speed = b2Length(newVelocity);
    if (speed > targetSpeed) {
        newVelocity = b2MulSV(targetSpeed / speed, newVelocity);
    }

    b2Body_SetLinearVelocity(body, newVelocity);

    // ===== ROTACIÓN HACIA LA DIRECCIÓN =====
    b2Rot currentRot = b2Body_GetRotation(body);
    float currentAngle = std::atan2(currentRot.s, currentRot.c);

    float angleDiff = targetAngle - currentAngle;

    // Normalizar la diferencia angular al rango [-π, π]
    while (angleDiff > 3.14159f) angleDiff -= 6.28318f;
    while (angleDiff < -3.14159f) angleDiff += 6.28318f;

    float angularVelocity = angleDiff * 3.0f;

    // Limitar velocidad angular
    float maxAngularVel = 5.0f;
    if (angularVelocity > maxAngularVel)
        angularVelocity = maxAngularVel;
    if (angularVelocity < -maxAngularVel)
        angularVelocity = -maxAngularVel;

    b2Body_SetAngularVelocity(body, angularVelocity);
}

b2Vec2 NPCTraffic::getPosition() const { return b2Body_GetPosition(body); }

b2Rot NPCTraffic::getRotation() const { return b2Body_GetRotation(body); }

b2Vec2 NPCTraffic::getLinearVelocity() const { return b2Body_GetLinearVelocity(body); }

void NPCTraffic::onCollision(Collidable* other, float approachSpeed, float deltaTime,
                             const b2Vec2& contactNormal) {
    // Calcular daño basado en velocidad de aproximación
    // Fórmula simple: 10 * (approachSpeed - 2.0)
    float damage = std::max(0.0f, (approachSpeed - 2.0f) * 10.0f);

    takeDamage(damage);

    // Incremente contador de colisiones para detectar esquinas/obstáculos
    collisionCount++;
    if (collisionCount > 2) {
        // Si hay múltiples colisiones, forzar cambio de dirección
        currentDirection = rand() % 4;
        collisionCount = 0;
    }
}

b2Rot NPCTraffic::getRotation(const b2Vec2& contactNormal) const { return getRotation(); }

void NPCTraffic::applyCollision(const CollisionInfo& info) {
    // Los NPCs aplican sus efectos de colisión si es necesario
    // Por ahora, solo el sistema de contactos maneja todo
}
