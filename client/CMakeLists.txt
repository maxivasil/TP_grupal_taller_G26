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
    checkpoint_arrow.cpp
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
    checkpoint_arrow.h
    UI/mainwindow.h
    )

target_link_libraries(taller_client 
    PRIVATE 
        client_cmd
)

# Link to Qt Widgets if it was found by the top-level CMake (guard against missing Qt)
if(DEFINED QT_VERSION_MAJOR AND TARGET Qt${QT_VERSION_MAJOR}::Widgets)
    target_link_libraries(taller_client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
endif()
