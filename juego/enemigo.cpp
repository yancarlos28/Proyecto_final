// enemigo.cpp
#include "enemigo.h"
#include <QDebug>
enemigo::enemigo()
    : personaje(80), enAlerta(false), fase(0) {}

void enemigo::actuar(float dt) {
    // Comportamiento base
    actualizar(dt);
}

void enemigo::debugPrint() const {
    qDebug() << "Enemigo pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()  // ✅ Usa getter
    << " alerta=" << enAlerta << " fase=" << fase;
}
