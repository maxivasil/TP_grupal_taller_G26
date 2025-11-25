#include "NPCTraffic.h"

#include <algorithm>
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

    // ===== SISTEMA DE RESPAWN POR ATORAMIENTO =====
    // Decrementar contador de colisiones si no hay colisiones
    collisionResetTimer += deltaTime;
    if (collisionResetTimer > COLLISION_RESET_TIME) {
        collisionResetTimer = 0.0f;
        if (totalCollisionsRecent > 0) {
            totalCollisionsRecent--;  // Reducir contador gradualmente
        }
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

        // PRIMERO: Detener al auto completamente
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
        
        // SEGUNDO: Cambiar dirección
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
        case 0:  // Norte (arriba, -Y)
            direction = {0.0f, -1.0f};
            targetAngle = -1.5708f;  // -π/2 radianes (apunta hacia arriba)
            break;
        case 1:  // Este (derecha, +X)
            direction = {1.0f, 0.0f};
            targetAngle = 0.0f;  // 0 radianes (apunta hacia la derecha)
            break;
        case 2:  // Sur (abajo, +Y)
            direction = {0.0f, 1.0f};
            targetAngle = 1.5708f;  // π/2 radianes (apunta hacia abajo)
            break;
        case 3:  // Oeste (izquierda, -X)
            direction = {-1.0f, 0.0f};
            targetAngle = 3.14159f;  // π radianes (apunta hacia la izquierda)
            break;
    }

    // Aplicar velocidad en dirección de movimiento
    b2Vec2 desiredVelocity = b2MulSV(targetSpeed, direction);

    // ===== GARANTIZAR MOVIMIENTO HACIA ADELANTE PURO =====
    // NO permitir que la física de Box2D cause retroceso
    // Aplicar SOLO la velocidad deseada, sin angularidad
    b2Body_SetLinearVelocity(body, desiredVelocity);
    
    // ===== ROTACIÓN INSTANTÁNEA =====
    // En lugar de usar velocidad angular (que causa problemas), rotamos al ángulo deseado instantáneamente
    // Esto asegura que el auto siempre apunta hacia donde se mueve
    
    // Crear la rotación deseada
    b2Rot desiredRot;
    desiredRot.c = std::cos(targetAngle);  // cos
    desiredRot.s = std::sin(targetAngle);  // sin
    
    // Aplicar transformación con la nueva rotación
    b2Transform transform;
    transform.p = b2Body_GetPosition(body);
    transform.q = desiredRot;
    b2Body_SetTransform(body, transform.p, transform.q);
    
    // Sin velocidad angular - el auto NO gira, solo se rota al ángulo objetivo
    b2Body_SetAngularVelocity(body, 0.0f);
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

    // ===== SISTEMA DE CONTEO DE COLISIONES =====
    // Incrementar contador total de colisiones recientes
    totalCollisionsRecent++;
    collisionResetTimer = 0.0f;  // Resetear timer cuando hay colisión

    // Si hay colisiones frecuentes, DETENER al auto y cambiar dirección
    if (totalCollisionsRecent > 3) {
        // Detener completamente
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
        b2Body_SetAngularVelocity(body, 0.0f);
        
        // Cambiar dirección aleatoriamente
        currentDirection = rand() % 4;
        totalCollisionsRecent = 0;
        
        std::cout << "[NPC] Demasiadas colisiones, cambiando dirección" << std::endl;
    }

    // Si supera el máximo, marcar para respawn
    if (totalCollisionsRecent > MAX_COLLISIONS_BEFORE_RESPAWN) {
        // El NPC necesita respawn - la carrera lo detectará y lo hará
        std::cout << "[NPC] NPC en posición (" << b2Body_GetPosition(body).x << ", " 
                  << b2Body_GetPosition(body).y << ") requiere respawn (colisiones: " 
                  << totalCollisionsRecent << ")" << std::endl;
        totalCollisionsRecent = 0;  // Resetear contador
    }
}

b2Rot NPCTraffic::getRotation(const b2Vec2& contactNormal) const { return getRotation(); }

void NPCTraffic::applyCollision(const CollisionInfo& info) {
    // Los NPCs aplican sus efectos de colisión si es necesario
    // Por ahora, solo el sistema de contactos maneja todo
}

void NPCTraffic::resetPosition(b2Vec2 newPos) {
    // Teleportear el NPC a nueva posición estableciendo velocidad basada en nueva posición
    // Aplicar un impulso que lo mueva a la nueva ubicación
    b2Vec2 currentPos = b2Body_GetPosition(body);
    b2Vec2 displacement = b2Sub(newPos, currentPos);
    
    // Resetear velocidades y rotación
    b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
    b2Body_SetAngularVelocity(body, 0.0f);
    
    // Aplicar impulso para mover el cuerpo
    float mass = b2Body_GetMass(body);
    if (mass > 0) {
        b2Vec2 impulse = b2MulSV(mass * 100.0f, displacement);  // Impulso grande para teleportación
        b2Body_ApplyLinearImpulse(body, impulse, currentPos, true);
    }
    
    // Resetear variables de movimiento
    lastPosition = newPos;
    stuckTimer = 0.0f;
    collisionCount = 0;
    totalCollisionsRecent = 0;
    collisionResetTimer = 0.0f;
    currentDirection = rand() % 4;
    
    std::cout << "[NPC] Respawn en posición (" << newPos.x << ", " << newPos.y << ")" << std::endl;
}

