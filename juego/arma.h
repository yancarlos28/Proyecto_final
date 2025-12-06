#ifndef ARMA_H
#define ARMA_H

#include "proyectil.h"
#include "modelomovimiento.h"
#include <cmath>

class Arma {

private:

    int danioBase;
    float velocidadProyectil;

    // Puntero original (se clona para cada proyectil)
    ModeloMovimiento* modelo;

public:

    // Constructor: arma con daño, velocidad y modelo de movimiento
    Arma(int dmg, float vel, ModeloMovimiento* model);

    // Destructor: libera el modelo base
    ~Arma();

    // Crea un nuevo proyectil con el modelo clonado
    proyectil* crearProyectil(float x, float y, float dirX, float dirY);

    // Getters
    int getDanio() const { return danioBase; }
    float getVelocidad() const { return velocidadProyectil; }

};

#endif
