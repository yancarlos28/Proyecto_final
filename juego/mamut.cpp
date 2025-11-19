// mamut.cpp
#include "mamut.h"
#include <QDebug>
mamut::mamut() {
    // ✅ Usa setter público en lugar de asignación directa
    setVida(200);
    rutaPatrulla = {0.0f, 200.0f, 400.0f};
}

void mamut::atacarAlJugador() {
    qDebug() << "[Mamut] Embiste al jugador!";
}

void mamut::actuar(float dt) {
    // Lógica de patrulla (usa getters/setters para posición)
    if (getVelX() == 0.0f) setVel(30.0f, 0);

    actualizar(dt); // Heredado de personaje

    // Cambiar dirección en límites (usa getters)
    if (getX() < 0.0f) {
        setPos(0.0f, getY());
        setVel(std::abs(getVelX()), getVelY());
    }
    if (getX() > 500.0f) {
        setPos(500.0f, getY());
        setVel(-std::abs(getVelX()), getVelY());
    }
}

void mamut::debugPrint() const {
    qDebug() << "Mamut pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()  // ✅ Usa getter
    << " rutaSize=" << rutaPatrulla.size();
}
