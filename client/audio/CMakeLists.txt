add_library(client_audio OBJECT
    car_sound_engine.cpp
    car_sound_engine.h
)

target_include_directories(client_audio
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(client_audio
    PRIVATE
    SDL2
    SDL2_mixer
)
