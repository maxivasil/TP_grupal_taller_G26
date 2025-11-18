add_subdirectory(cmd)
add_subdirectory(graphics)
add_subdirectory(audio)

target_sources(nfs2d_client
    PRIVATE
    # .cpp files
    main.cpp
    parser.cpp
    client_receiver.cpp
    client_sender.cpp
    session.cpp
    client_game.cpp
    UI/resources.qrc
    UI/mainwindow.cpp
    UI/mainwindow.ui
    UI/initialwindow.cpp
    UI/initialwindow.ui
    UI/trackwindow.cpp
    UI/trackwindow.ui
    UI/readywindow.cpp
    UI/readywindow.ui
    PUBLIC
    # .h files
    parser.h
    client_receiver.h
    client_sender.h
    session.h
    client_game.h
    UI/mainwindow.h
    UI/initialwindow.h
    UI/trackwindow.h
    UI/readywindow.h
    )

target_link_libraries(nfs2d_client 
    PRIVATE 
        client_cmd
        client_graphics
        client_audio
        yaml-cpp
)

# Link to Qt Widgets if it was found by the top-level CMake (guard against missing Qt)
if(DEFINED QT_VERSION_MAJOR AND TARGET Qt${QT_VERSION_MAJOR}::Widgets)
    target_link_libraries(nfs2d_client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
endif()
