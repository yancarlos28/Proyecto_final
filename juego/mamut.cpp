#include "mamut.h"
#include <iostream>
mamut::mamut() {
    vida = 200;
    // ejemplo de ruta de patrulla (x positions)
    rutaPatrulla = {0.0f, 200.0f, 400.0f};
}

void mamut::atacarAlJugador() {
    std::cout << "[Mamut] embiste!\n";
}

void mamut::actuar(float dt) {
    // ejemplo simple de patrulla: avanzar en X según velocidad
    // si no hay velocidad, le damos una por defecto:
    if (velX == 0.0f) velX = 30.0f;
    actualizar(dt);

    // lógica simplificada: cambiar dirección al alcanzar límites
    if (posX < 0.0f) { posX = 0.0f; velX = std::abs(velX); }
    if (posX > 500.0f) { posX = 500.0f; velX = -std::abs(velX); }
}

void mamut::debugPrint() const {
    std::cout << "Mamut pos=(" << posX << "," << posY << ") vida=" << vida
              << " rutaSize=" << rutaPatrulla.size() << "\n";
}
