// mamut.cpp
#include "mamut.h"
#include <QDebug>

mamut::mamut(): indiceObjetivo(0),velocidad(300)  
{
    setVida(200);

    // Ruta mamut
    rutaPatrulla = {0, 200, 400};

    if (!rutaPatrulla.empty()) {
        setPos(rutaPatrulla[0], 720);
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

void mamut::actuar(float dt)
{
    if (rutaPatrulla.empty()) {
        actualizar(dt);
        return;
    }

    float xActual  = getX();
    float xDestino = rutaPatrulla[indiceObjetivo];

    // ¿Llegó (o casi) al punto actual?
    if (std::fabs(xDestino - xActual) < 5.0) {
        indiceObjetivo = (indiceObjetivo + 1) % rutaPatrulla.size();
        xDestino = rutaPatrulla[indiceObjetivo];
    }

    float direccion = (xDestino > xActual) ? 1.0 : -1.0;
    setVel(velocidad * direccion, 0.0f);

    // Avanzar con la física base de personaje
    actualizar(dt);
}

void mamut::atacarAlJugador() {
    qDebug() << "[Mamut] Embiste al jugador!";
}
void mamut::debugPrint() const {
    qDebug() << "Mamut pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()
    << " rutaSize=" << rutaPatrulla.size()
    << " objetivo=" << indiceObjetivo;
}
