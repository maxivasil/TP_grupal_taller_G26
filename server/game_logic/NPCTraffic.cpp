#include "NPCTraffic.h"
#include <cmath>

b2BodyDef NPCTraffic::initNPCBodyDef(b2Vec2 position) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.userData = this;
    bodyDef.isAwake = true;
    bodyDef.linearDamping = 0.7f;   // Damping similar a los autos jugadores
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

NPCTraffic::NPCTraffic(b2WorldId world, uint8_t carType_, b2Vec2 position)
    : carType(carType_), current_health(100.0f), maxHealth(100.0f) {
    
    b2BodyDef bodyDef = initNPCBodyDef(position);
    body = b2CreateBody(world, &bodyDef);
    
    setShape(body);
}

NPCTraffic::~NPCTraffic() {
    // El body se destruye cuando se destruye el world
}

void NPCTraffic::updatePhysics(float deltaTime, const std::vector<RoutePoint>* route) {
    // Si no se proporciona ruta, usar la asignada
    if (route == nullptr) {
        route = assignedRoute;
    }
    
    // Si no hay ruta o el NPC está estacionado, no hacer nada
    if (route == nullptr || route->empty() || isDestroyed()) {
        return;
    }
    
    size_t currentIdx = currentRoutePoint % route->size();
    size_t nextIdx = (currentIdx + 1) % route->size();
    
    const RoutePoint& current = (*route)[currentIdx];
    const RoutePoint& next = (*route)[nextIdx];
    
    // Velocidad objetivo según la ruta
    targetSpeed = current.speed;
    
    // Obtener posición actual
    b2Vec2 currentPos = b2Body_GetPosition(body);
    
    // Vector al siguiente waypoint
    b2Vec2 targetPos = {next.x, next.y};
    b2Vec2 direction = b2Sub(targetPos, currentPos);
    float distance = b2Length(direction);
    
    // Si llegamos al waypoint, avanzar
    if (distance < 0.5f) {
        currentRoutePoint = (currentRoutePoint + 1) % route->size();
        return;
    }
    
    // Normalizar dirección
    if (distance > 0.0f) {
        direction.x /= distance;
        direction.y /= distance;
    }
    
    // Aplicar velocidad en dirección de movimiento
    b2Vec2 desiredVelocity = b2MulSV(targetSpeed, direction);
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(body);
    
    // Suavizar cambios de velocidad
    b2Vec2 velocityChange = b2Sub(desiredVelocity, currentVelocity);
    velocityChange = b2MulSV(0.5f, velocityChange);  // Suavizado
    
    b2Vec2 newVelocity = b2Add(currentVelocity, velocityChange);
    
    // Limitar a velocidad máxima
    float speed = b2Length(newVelocity);
    if (speed > targetSpeed) {
        newVelocity = b2MulSV(targetSpeed / speed, newVelocity);
    }
    
    b2Body_SetLinearVelocity(body, newVelocity);
    
    // ROTACIÓN: Hacer que el auto gire para apuntar en la dirección de movimiento
    // Calcular el ángulo deseado (hacia dónde debería apuntar el auto)
    float targetAngle = std::atan2(direction.y, direction.x);
    
    // Obtener rotación actual del auto
    b2Rot currentRot = b2Body_GetRotation(body);
    float currentAngle = std::atan2(currentRot.s, currentRot.c);
    
    // Calcular diferencia angular (en radianes)
    float angleDiff = targetAngle - currentAngle;
    
    // Normalizar la diferencia al rango [-π, π]
    while (angleDiff > 3.14159f) angleDiff -= 6.28318f;
    while (angleDiff < -3.14159f) angleDiff += 6.28318f;
    
    // Aplicar torque para girar el auto suavemente
    // Factor de giro: cuanto mayor, más rápido gira
    float angularVelocity = angleDiff * 3.0f;  // Control de velocidad angular
    
    // Limitar velocidad angular
    float maxAngularVel = 5.0f;  // Radianes por segundo
    if (angularVelocity > maxAngularVel) angularVelocity = maxAngularVel;
    if (angularVelocity < -maxAngularVel) angularVelocity = -maxAngularVel;
    
    b2Body_SetAngularVelocity(body, angularVelocity);
}

b2Vec2 NPCTraffic::getPosition() const {
    return b2Body_GetPosition(body);
}

b2Rot NPCTraffic::getRotation() const {
    return b2Body_GetRotation(body);
}

b2Vec2 NPCTraffic::getLinearVelocity() const {
    return b2Body_GetLinearVelocity(body);
}

void NPCTraffic::onCollision(Collidable* other, float approachSpeed, float deltaTime,
                             const b2Vec2& contactNormal) {
    // Calcular daño basado en velocidad de aproximación
    // Fórmula simple: 10 * (approachSpeed - 2.0)
    float damage = std::max(0.0f, (approachSpeed - 2.0f) * 10.0f);
    
    takeDamage(damage);
}

b2Rot NPCTraffic::getRotation(const b2Vec2& contactNormal) const {
    return getRotation();
}

void NPCTraffic::applyCollision(const CollisionInfo& info) {
    // Los NPCs aplican sus efectos de colisión si es necesario
    // Por ahora, solo el sistema de contactos maneja todo
}
