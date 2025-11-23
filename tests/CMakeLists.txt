set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(tests
    main_test.cpp
    test_accumulatedResults_deserialization.cpp
    test_accumulatedResults_serialization.cpp
    test_snapshot.cpp

    testable_server_to_client_accumulatedResults.h
)

target_link_libraries(tests 
    PRIVATE 
        server_cmd
        server_game_logic
        server_core
        taller_common
        client_cmd
        client_graphics
        client_audio
        client_core
        GTest::gtest_main
        Qt${QT_VERSION_MAJOR}::Widgets
        SDL2pp::SDL2pp
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf        
        yaml-cpp
        box2d::box2d
)
