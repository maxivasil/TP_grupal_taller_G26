#include "game_state.h"

void GameState::update_from_world(b2WorldId worldId) {
    cars_.clear();

    int bodyCount = 0;
    b2BodyId* bodies = b2World_GetBodies(worldId, &bodyCount);

    for (int i = 0; i < bodyCount; ++i) {
        b2BodyId bodyId = bodies[i];
        
        // TODO: filtrar por autos (checkear user data o tipo de body)
        
        CarState cs;
        cs.id = static_cast<uint32_t>(bodyId.index1); // usar el ID interno de Box2D
        
        b2Vec2 position = b2Body_GetPosition(bodyId);
        cs.x = position.x;
        cs.y = position.y;
        cs.angle = b2Body_GetRotation(bodyId).c; // en v3, la rotación es un b2Rot (cos/sin)
        
        b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
        cs.speed = b2Length(velocity);
        
        cs.health = 100.0f; // placeholder
        cars_.push_back(cs);
    }
}

const std::vector<CarState>& GameState::get_cars() const {
    return cars_;
}

void GameState::clear() {
    cars_.clear();
}
