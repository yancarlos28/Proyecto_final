#ifndef NIVEL_H
#define NIVEL_H

#include "caverman.h"


class Nivel
{
public:
    virtual ~Nivel() = default;

    // Se llamará en cada frame para actualizar la lógica del nivel
    virtual void actualizar(float dt) = 0;

    // Acceso al jugador (caverman)
    caverman& getJugador() { return jugador; }
    const caverman& getJugador() const { return jugador; }

protected:
    // Jugador del nivel
    caverman jugador;
};

#endif // NIVEL_H

