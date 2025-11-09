add_library(server_cmd
    cmd_server_register.h
    cmd_base_server.h
    client_to_server_move.h
    server_to_client_snapshot.h
    client_to_server_joinLobby.h
    server_to_client_joinLobbyResponse.h
    client_to_server_readyToStart.h
    client_to_server_cheat.h

    cmd_server_register.cpp
    cmd_base_server.cpp
    client_to_server_move.cpp
    server_to_client_snapshot.cpp
    client_to_server_joinLobby.cpp
    server_to_client_joinLobbyResponse.cpp
    client_to_server_readyToStart.cpp
    client_to_server_cheat.cpp
)

target_link_libraries(server_cmd PRIVATE server_game_logic taller_common)

target_include_directories(server_cmd 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/server/game_logic
    )
