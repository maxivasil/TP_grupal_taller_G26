add_subdirectory(game_logic)
add_subdirectory(cmd)

target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    client_handler.cpp
    gameloop.cpp
    main.cpp
    protected_clients.cpp
    session.cpp
    server_receiver.cpp
    server_sender.cpp
    
    PUBLIC
    # .h files
    acceptor.h
    client_handler.h
    gameloop.h
    protected_clients.h
    session.h
    server_receiver.h
    server_sender.h
    
    )

target_link_libraries(taller_server 
    PRIVATE 
        server_cmd
        server_game_logic
    )
