add_library(server_game_logic
    Car.h
    NPCCar.h
    NPCTraffic.h
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
    IntersectionManager.h

    Car.cpp
    NPCCar.cpp
    NPCTraffic.cpp
    PhysicsEngine.cpp
    WorldContactListener.cpp
    StaticObject.cpp
    CheckpointManager.cpp
    Player.cpp
    Track.cpp
    City.cpp
    BridgeSensorManager.cpp
    Race.cpp
    IntersectionManager.cpp
)

# Agregar subdirectorio de NPC checkpoint spawning
# TODO: Arreglar errores de compilación en este módulo
# add_subdirectory(npc_checkpoint_spawning)

# target_link_libraries(server_game_logic PUBLIC box2d::box2d yaml-cpp npc_checkpoint_spawning)
target_link_libraries(server_game_logic PUBLIC box2d::box2d yaml-cpp)

