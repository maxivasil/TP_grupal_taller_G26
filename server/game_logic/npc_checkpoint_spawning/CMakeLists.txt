# Library: NPC Checkpoint Spawning
add_library(npc_checkpoint_spawning STATIC
    CheckpointSpawner.cpp
    CheckpointSpawner.h
    NPCCheckpointManager.cpp
    NPCCheckpointManager.h
)

target_compile_features(npc_checkpoint_spawning PRIVATE cxx_std_17)

# Include directories
target_include_directories(npc_checkpoint_spawning PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(npc_checkpoint_spawning PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Dependencies
target_link_libraries(npc_checkpoint_spawning PUBLIC box2d::box2d yaml-cpp)
