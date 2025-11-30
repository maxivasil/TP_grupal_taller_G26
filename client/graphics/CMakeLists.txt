add_library(client_graphics OBJECT
    camera.cpp
    camera.h
    hud.cpp
    hud.h
    minimap.cpp
    minimap.h
    checkpoint_arrow.cpp
    checkpoint_arrow.h
    collision_explosion.cpp
    collision_explosion.h
    car_fire_effect.cpp
    car_fire_effect.h
    track_loader.cpp
    track_loader.h
)

target_include_directories(client_graphics
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(client_graphics
    PRIVATE
    SDL2pp
    yaml-cpp
)
