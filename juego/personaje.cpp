#include "personaje.h"
#include <QDebug>

personaje::personaje(int vidaInicial)
    : vida(vidaInicial), posX(0), posY(0), velX(0), velY(0) {}

personaje::~personaje() = default;

void personaje::mover(float dx, float dy) {
    posX += dx;
    posY += dy;
}

void personaje::actualizar(float dt) {
    posX += velX * dt;
    posY += velY * dt;
}

void personaje::debugPrint() const {
    qDebug() << "Personaje pos=(" << getX() << "," << getY() << ") vida=" << getVida();
}
