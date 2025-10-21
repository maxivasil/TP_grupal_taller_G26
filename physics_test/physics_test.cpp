#include "physics.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== TEST: PhysicsEngine Wrapper ===" << std::endl;

    PhysicsEngine physics;
    physics.addCar("player1", 0.0f, 0.0f);
    physics.addWall(5.0f, 0.0f, 1.0f, 3.0f);

    // Aplicar impulso inicial hacia el muro
    physics.applyImpulse("player1", 10.0f, 0.0f);

    for (int i = 0; i < 120; ++i) {
        physics.step(1.0f / 60.0f);
        auto [x, y] = physics.getPosition("player1");

        std::cout << std::fixed << std::setprecision(2)
                  << "Paso " << std::setw(3) << i
                  << " | Pos(" << x << ", " << y << ")"
                  << std::endl;
    }

    std::cout << "=== Fin del test ===" << std::endl;
    return 0;
}
