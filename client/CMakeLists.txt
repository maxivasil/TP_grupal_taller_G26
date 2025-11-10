add_subdirectory(cmd)

target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    camera.cpp
    client_game.cpp
    parser.cpp
    client_receiver.cpp
    client_sender.cpp
    session.cpp
    minimap.cpp
    hud.cpp
    hints.cpp
    UI/resources.qrc
    UI/mainwindow.cpp
    PUBLIC
    # .h files
    camera.h
    client_game.h
    parser.h
    client_receiver.h
    client_sender.h
    session.h
    minimap.h
    hud.h
    hints.h
    UI/mainwindow.h
    )

target_link_libraries(taller_client 
    PRIVATE 
        client_cmd
        Qt${QT_VERSION_MAJOR}::Widgets
    )
