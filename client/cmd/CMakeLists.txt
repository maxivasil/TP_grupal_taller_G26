add_library(client_cmd
    cmd_client_register.h
    cmd_base_client.h
    client_to_server_move.h
    server_to_client_snapshot.h
    
    cmd_client_register.cpp
    cmd_base_client.cpp
    client_to_server_move.cpp
    server_to_client_snapshot.cpp  
)

target_include_directories(client_cmd 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR})
