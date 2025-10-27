add_library(server_cmd
    cmd_server_register.h
    cmd_base_server.h
    client_to_server_move.h

    cmd_server_register.cpp
    cmd_base_server.cpp
    client_to_server_move.cpp
)

target_include_directories(server_cmd 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR})
