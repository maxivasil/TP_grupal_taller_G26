set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(client_cmd
    ../client_game.h
    cmd_client_register.h
    cmd_base_client.h
    client_to_server_move.h
    server_to_client_snapshot.h
    client_to_server_lobby.h
    server_to_client_lobbyResponse.h
    server_to_client_gameStarting.h   
    client_to_server_readyToStart.h
    client_to_server_cheat.h
    server_to_client_raceResults.h
    server_to_client_assign_id.h
    client_to_server_tour.h
    
    ../client_game.cpp
    cmd_client_register.cpp
    cmd_base_client.cpp
    client_to_server_move.cpp
    server_to_client_snapshot.cpp
    server_to_client_gameStarting.cpp  
    client_to_server_lobby.cpp
    server_to_client_lobbyResponse.cpp
    client_to_server_readyToStart.cpp 
    client_to_server_cheat.cpp
    server_to_client_raceResults.cpp
    server_to_client_assign_id.cpp
    client_to_server_tour.cpp
)

target_link_libraries(client_cmd PUBLIC Qt${QT_VERSION_MAJOR}::Widgets SDL2pp::SDL2pp taller_common)

target_include_directories(client_cmd 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}          # ya que taller_common PUBLIC incluyó "."
        ${CMAKE_SOURCE_DIR}/common   # explícito por si incluís "buffer_utils.h"
)
