target_sources(taller_common
    PRIVATE
    # .cpp files
    buffer_utils.cpp
    liberror.cpp
    protocol.cpp
    resolver.cpp
    resolvererror.cpp
    socket.cpp
    PUBLIC
    # .h files
    buffer_utils.h
    liberror.h
    protocol.h
    queue.h
    resolver.h
    resolvererror.h
    socket.h
    thread.h
    )
