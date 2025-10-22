#include "physics.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== TEST: PhysicsEngine con detección de colisiones ===" << std::endl;

    PhysicsEngine physics;
    physics.addCar("AutoJugador", 0.0f, 0.0f);
    physics.addWall(5.0f, 0.0f, 1.0f, 3.0f);

    physics.applyImpulse("AutoJugador", 10.0f, 0.0f);

    for (int i = 0; i < 120; ++i) {
        physics.step(1.0f / 60.0f);
        auto [x, y] = physics.getPosition("AutoJugador");

        std::cout << std::fixed << std::setprecision(2)
                  << "Paso " << std::setw(3) << i
                  << " | Pos(" << x << ", " << y << ")"
                  << std::endl;
    }

    std::cout << "=== Fin del test ===" << std::endl;
    return 0;
}
