add_library(server_cmd
    cmd_server_register.h
    cmd_base_server.h
    client_to_server_move_server.h
    server_to_client_snapshot_server.h
    client_to_server_joinLobby_server.h
    server_to_client_joinLobbyResponse_server.h
    client_to_server_readyToStart_server.h
    server_to_client_gameStarting_server.h
    client_to_server_cheat_server.h
    server_to_client_raceResults_server.h
    server_to_client_assign_id_server.h
    client_to_server_tour_server.h
    server_to_client_startingRace_server.h
    client_to_server_applyUpgrades_server.h
    server_to_client_accumulatedResults_server.h

    cmd_server_register.cpp
    cmd_base_server.cpp
    client_to_server_move_server.cpp
    server_to_client_snapshot_server.cpp
    client_to_server_joinLobby_server.cpp
    server_to_client_joinLobbyResponse_server.cpp
    client_to_server_readyToStart_server.cpp
    server_to_client_gameStarting_server.cpp  
    client_to_server_cheat_server.cpp
    server_to_client_raceResults_server.cpp
    server_to_client_assign_id_server.cpp
    client_to_server_tour_server.cpp
    server_to_client_startingRace_server.cpp
    client_to_server_applyUpgrades_server.cpp
    server_to_client_accumulatedResults_server.cpp
)

target_link_libraries(server_cmd PRIVATE server_game_logic taller_common)

target_include_directories(server_cmd 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/server/game_logic
    )
