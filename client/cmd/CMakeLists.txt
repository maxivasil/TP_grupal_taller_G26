set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(client_cmd
    ../client_game.h
    cmd_client_register.h
    cmd_base_client.h
    client_to_server_move_client.h
    server_to_client_snapshot_client.h
    client_to_server_joinLobby_client.h
    server_to_client_joinLobbyResponse_client.h
    server_to_client_gameStarting_client.h   
    client_to_server_readyToStart_client.h
    client_to_server_cheat_client.h
    server_to_client_raceResults_client.h
    server_to_client_assign_id_client.h
    client_to_server_tour_client.h
    server_to_client_startingRace_client.h
    server_to_client_accumulatedResults_client.h
    client_to_server_applyUpgrades_client.h
    
    ../client_game.cpp
    cmd_client_register.cpp
    cmd_base_client.cpp
    client_to_server_move_client.cpp
    server_to_client_snapshot_client.cpp
    server_to_client_gameStarting_client.cpp  
    client_to_server_joinLobby_client.cpp
    server_to_client_joinLobbyResponse_client.cpp
    client_to_server_readyToStart_client.cpp 
    client_to_server_cheat_client.cpp
    server_to_client_raceResults_client.cpp
    server_to_client_assign_id_client.cpp
    client_to_server_tour_client.cpp
    server_to_client_startingRace_client.cpp
    server_to_client_accumulatedResults_client.cpp
    client_to_server_applyUpgrades_client.cpp
)

target_link_libraries(client_cmd PUBLIC Qt${QT_VERSION_MAJOR}::Widgets SDL2pp::SDL2pp taller_common)

target_include_directories(client_cmd 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}          # ya que taller_common PUBLIC incluyó "."
        ${CMAKE_SOURCE_DIR}/common   # explícito por si incluís "buffer_utils.h"
)
