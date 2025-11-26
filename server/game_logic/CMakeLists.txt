add_library(server_game_logic
    Car.h
    NPCCar.h
    PhysicsEngine.h
    WorldContactListener.h
    StaticObject.h
    CollisionInfo.h
    Collidable.h
    PhysicalCheckpoint.h
    CheckpointManager.h
    Player.h
    Track.h
    City.h
    SensorData.h
    BridgeSensorManager.h
    Race.h
    NPCTraffic.h

    Car.cpp
    NPCCar.cpp
    PhysicsEngine.cpp
    WorldContactListener.cpp
    StaticObject.cpp
    CheckpointManager.cpp
    Player.cpp
    Track.cpp
    City.cpp
    BridgeSensorManager.cpp
    Race.cpp
    NPCTraffic.cpp
)

target_link_libraries(server_game_logic PUBLIC box2d::box2d yaml-cpp)

