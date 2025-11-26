add_subdirectory(cmd)
add_subdirectory(graphics)
add_subdirectory(audio)

add_library(client_core
    # .cpp files
    parser.cpp
    client_receiver.cpp
    client_sender.cpp
    session.cpp
    client_game.cpp
    UI/mainwindow.cpp
    UI/mainwindow.ui
    UI/initialwindow.cpp
    UI/initialwindow.ui
    UI/readywindow.cpp
    UI/readywindow.ui
    UI/trackwindow.cpp
    UI/trackwindow.ui
    UI/resources.qrc

    # .h files
    parser.h
    client_receiver.h
    client_sender.h
    session.h
    client_game.h
    UI/mainwindow.h
    UI/initialwindow.h
    UI/readywindow.h
    UI/trackwindow.h
)

target_link_libraries(client_core
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        SDL2pp::SDL2pp 
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
)

target_sources(nfs2d_client
    PRIVATE
        main.cpp
)

target_link_libraries(nfs2d_client 
    PRIVATE 
        client_core
        client_cmd
        client_graphics
        client_audio
        yaml-cpp
)

# Link to Qt Widgets if it was found by the top-level CMake (guard against missing Qt)
if(DEFINED QT_VERSION_MAJOR AND TARGET Qt${QT_VERSION_MAJOR}::Widgets)
    target_link_libraries(nfs2d_client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
endif()
