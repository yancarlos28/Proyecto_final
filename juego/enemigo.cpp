#include "enemigo.h"
#include <iostream>

enemigo::enemigo()
    : personaje(80), enAlerta(false), fase(0) {}

void enemigo::actuar(float dt) {
    // comportamiento base: simplemente se actualiza
    actualizar(dt);
}

void enemigo::debugPrint() const {
    std::cout << "Enemigo pos=(" << posX << "," << posY << ") vida=" << vida
              << " alerta=" << enAlerta << " fase=" << fase << "\n";
}
