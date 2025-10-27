add_library(server_cmd
    cmd_server_register.h
    cmd_base_server.h
    client_to_server_move.h
    server_to_client_snapshot.h

    cmd_server_register.cpp
    cmd_base_server.cpp
    client_to_server_move.cpp
    server_to_client_snapshot.cpp
)

target_link_libraries(server_cmd PRIVATE taller_common)

target_include_directories(server_cmd 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common    
    )
