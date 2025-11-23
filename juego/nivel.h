#ifndef NIVEL_H
#define NIVEL_H

#include "caverman.h"
#include "proyectil.h"
#include <vector>

class Nivel
{
public:
    virtual ~Nivel() = default;


    virtual void actualizar(float dt) = 0;
    virtual bool estaCompletado() const { return false; }
    virtual bool estaFallido() const { return false; }

    // Acceso al jugador (caverman)
    caverman& getJugador();
    const caverman& getJugador() const;

    //metodo para proyectiles
    virtual const std::vector<proyectil*>& getProyectiles() const;

protected:
    caverman jugador;
};

#endif // NIVEL_H

