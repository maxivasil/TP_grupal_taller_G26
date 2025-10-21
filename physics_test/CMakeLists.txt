# Wrapper test para PhysicsEngine
add_executable(physics_test
    physics.cpp
    physics_test.cpp
)

target_include_directories(physics_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(physics_test PRIVATE box2d)