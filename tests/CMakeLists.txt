file(GLOB TEST_SOURCES
    "*.cpp"
)

target_sources(taller_tests PRIVATE
    ${TEST_SOURCES}

    ${CMAKE_SOURCE_DIR}/common/buffer_utils.cpp
    ${CMAKE_SOURCE_DIR}/client/cmd/server_to_client_accumulatedResults.cpp
)

target_include_directories(taller_tests PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/client/cmd
    ${gtest_SOURCE_DIR}/include
)

target_link_libraries(taller_tests
    PRIVATE
        taller_common
        GTest::gtest_main
)
