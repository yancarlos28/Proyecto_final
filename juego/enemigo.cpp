#include "enemigo.h"
#include <QDebug>

enemigo::enemigo()
    : personaje(80), enAlerta(false), fase(0) {}

bool enemigo::getAlerta() const {
    return enAlerta; }

void enemigo::setAlerta(bool alerta) {
    enAlerta = alerta; }

void enemigo::setFase(int f) {
    fase = f; }

void enemigo::actuar(float dt) {
    actualizar(dt);
}

void enemigo::debugPrint() const {
    qDebug() << "Enemigo pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()
    << " alerta=" << enAlerta << " fase=" << fase;
}
