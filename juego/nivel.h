#ifndef NIVEL_H
#define NIVEL_H

#include "caverman.h"
#include "proyectil.h"
#include <vector>

class Nivel
{
public:

    virtual ~Nivel();

    // Actualización lógica
    virtual void actualizar(float dt) = 0;


    // Acceso al jugador (caverman)
    caverman& getJugador();
    const caverman& getJugador() const;

    // Proyectiles: se separan los del jugador y los enemigos
    const std::vector<proyectil*>& getProyectilesJugador() const;
    const std::vector<proyectil*>& getProyectilesEnemigos() const;

    // Método genérico (compatibilidad): devuelve proyectiles de enemigos
    virtual const std::vector<proyectil*>& getProyectiles() const;

protected:

    caverman jugador;

    // Listas comunes a todos los niveles
    std::vector<proyectil*> proyectilesJugador;
    std::vector<proyectil*> proyectilesEnemigos;
};

#endif // NIVEL_H

