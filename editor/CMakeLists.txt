set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

target_sources(taller_editor
    PRIVATE
    # .cpp files
    main.cpp
    mainwindow.cpp
    mapview.cpp

    PUBLIC
    # .h files
    mainwindow.h
    mapview.h
    checkpoint.h
    )
