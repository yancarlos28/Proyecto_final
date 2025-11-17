#include "caverman.h"
#include <iostream>

caverman::caverman()
    : personaje(120), energia(100), puntuacion(0) {}

void caverman::lanzar() {
    // aquí solo notificar; el Nivel creará el proyectil
    std::cout << "[Caverman] lanzar() -> crear proyectil desde el nivel\n";
}

void caverman::curar(int valor) {
    energia += valor;
    if (energia > 100) energia = 100;
}

void caverman::debugPrint() const {
    std::cout << "Caverman pos=(" << posX << "," << posY << ") vida=" << vida
              << " energia=" << energia << " pts=" << puntuacion << "\n";
}
