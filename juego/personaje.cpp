#include "personaje.h"
#include <iostream>

personaje::personaje(int vidaInicial)
    : vida(vidaInicial), posX(0.0f), posY(0.0f), velX(0.0f), velY(0.0f) {}

personaje::~personaje() = default;

void personaje::mover(float dx, float dy) {
    posX += dx;
    posY += dy;
}

void personaje::actualizar(float dt) {
    // movimiento básico por velocidad
    posX += velX * dt;
    posY += velY * dt;
}

void personaje::recibirDanio(int valor) {
    vida -= valor;
    if (vida < 0) vida = 0;
}

void personaje::debugPrint() const {
    std::cout << "Personaje pos=(" << posX << "," << posY << ") vida=" << vida << "\n";
}
