#include "personaje.h"
#include <QDebug>

personaje::personaje(int vidaInicial)
    : vida(vidaInicial), posX(0), posY(0), velX(0), velY(0) {}

int personaje::getVida() const { return vida; }

float personaje::getX() const { return posX; }

float personaje::getY() const { return posY; }

float personaje::getVelX() const { return velX; }

float personaje::getVelY() const { return velY; }

void personaje::setVida(int v) {
    vida = (v < 0) ? 0 : v; }

void personaje::setPos(float x, float y) {
    posX = x;
    posY = y; }

void personaje::setVel(float vx, float vy) {
    velX = vx;
    velY = vy; }

personaje::~personaje() = default;


void personaje::actualizar(float dt) {
    posX += velX * dt;
    posY += velY * dt;
}

void personaje::debugPrint() const {
    qDebug() << "Personaje pos=(" << getX() << "," << getY() << ") vida=" << getVida();
}
