add_subdirectory(game_logic)
add_subdirectory(cmd)

add_library(server_core
    # .cpp files
    acceptor.cpp
    client_handler.cpp
    gameloop.cpp
    protected_clients.cpp
    session.cpp
    server_receiver.cpp
    server_sender.cpp
    LobbiesMonitor.cpp

    # .h files
    acceptor.h
    client_handler.h
    gameloop.h
    protected_clients.h
    session.h
    server_receiver.h
    server_sender.h
    LobbiesMonitor.h
    Lobby.h
    LobbyStatus.h
)

target_link_libraries(server_core PUBLIC box2d::box2d yaml-cpp)

target_sources(nfs2d_server
    PRIVATE
        main.cpp
)

target_link_libraries(nfs2d_server 
    PRIVATE 
        server_cmd
        server_game_logic
        server_core
    )
