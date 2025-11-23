set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(tests
    main_test.cpp
    test_accumulatedResults_deserialization.cpp
    test_accumulatedResults_serialization.cpp
    test_assign_id_serialization.cpp
    test_assing_id_deserialization.cpp
    test_lobbyResponse_serialization.cpp
    test_lobbyResponse_deserialization.cpp
    test_raceResults_serialization.cpp
    test_raceResults_deserialization.cpp
    test_snapshot_serialization.cpp
    test_snapshot_deserialization.cpp
    test_startingRace_serialization.cpp
    test_startingRace_deserialization.cpp
    
    testable_server_to_client.h
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
