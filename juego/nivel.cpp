#include "nivel.h"

//Nivel::Nivel() {}

caverman &Nivel::getJugador() {
    return jugador; }

const caverman &Nivel::getJugador() const {
    return jugador; }

const std::vector<proyectil *> &Nivel::getProyectiles() const {
    static std::vector<proyectil*> vacio;
    return vacio;
}
