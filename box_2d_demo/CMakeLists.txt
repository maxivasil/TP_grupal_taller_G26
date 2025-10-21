# Demo de prueba de Box2D
add_executable(box2d_demo box2d_demo.cpp)

# Enlazar la librería de Box2D (instalada en el sistema)
target_link_libraries(box2d_demo PRIVATE box2d)
