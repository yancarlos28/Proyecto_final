// mamut.cpp
#include "mamut.h"
#include <QDebug>

mamut::mamut()
    : indiceObjetivo(0),
    velocidad(150)      // velocidad base
{
    setVida(200);           // Usa setter heredado

    // Ruta por defecto (por si no se configura otra)
    rutaPatrulla = {0, 200, 400};

    if (!rutaPatrulla.empty()) {
        setPos(rutaPatrulla[0], 720.0f); // mismo “piso” que el caverman
    }
}
void mamut::setRuta(const std::vector<float>& ruta)
{
    rutaPatrulla = ruta;
    indiceObjetivo = 0;
    if (!rutaPatrulla.empty()) {
        setPos(rutaPatrulla[0], getY());
    }
}
void mamut::atacarAlJugador() {
    qDebug() << "[Mamut] Embiste al jugador!";
}

void mamut::actuar(float dt)
{
    if (rutaPatrulla.empty()) {
        actualizar(dt); // solo avanza con su velocidad actual
        return;
    }

    float xActual  = getX();
    float xDestino = rutaPatrulla[indiceObjetivo];

    // ¿Llegó (o casi) al punto actual?
    if (std::fabs(xDestino - xActual) < 5.0f) {
        indiceObjetivo = (indiceObjetivo + 1) % rutaPatrulla.size();
        xDestino = rutaPatrulla[indiceObjetivo];
    }

    float direccion = (xDestino > xActual) ? 1.0f : -1.0f;
    setVel(velocidad * direccion, 0.0f); // usa setVel heredado

    // Avanzar con la física base de personaje
    actualizar(dt);
}

void mamut::debugPrint() const {
    qDebug() << "Mamut pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()
    << " rutaSize=" << rutaPatrulla.size()
    << " objetivo=" << indiceObjetivo;
}
