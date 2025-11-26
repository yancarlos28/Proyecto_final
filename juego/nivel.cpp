#include "nivel.h"

Nivel::~Nivel()
{
    for (proyectil* p : proyectilesJugador) {
        delete p;
    }
    proyectilesJugador.clear();

    for (proyectil* p : proyectilesEnemigos) {
        delete p;
    }
    proyectilesEnemigos.clear();
}
const std::vector<proyectil*>& Nivel::getProyectilesJugador() const
{
    return proyectilesJugador;
}

const std::vector<proyectil*>& Nivel::getProyectilesEnemigos() const
{
    return proyectilesEnemigos;
}
const std::vector<proyectil*>& Nivel::getProyectiles() const {
    // Por defecto, usamos los proyectiles de los enemigos
    return proyectilesEnemigos;
}
caverman &Nivel::getJugador()
{
    return jugador;
}

const caverman &Nivel::getJugador() const
{
    return jugador;
}
