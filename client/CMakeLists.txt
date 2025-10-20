target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    parser.cpp
    client_receiver.cpp
    client_sender.cpp
    session.cpp
    PUBLIC
    # .h files
    parser.h
    client_receiver.h
    client_sender.h
    session.h
    )
