#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "modelomovimiento.h"
#include <memory>


class proyectil {

private:

    float posX, posY;
    float velX, velY;
    int danio;

    //Cada proyectil es dueño de su modelo de movimiento
    std::unique_ptr<ModeloMovimiento> modelo;

public:

    //constructor
    proyectil(float x, float y, float vx, float vy, int dmg, ModeloMovimiento* mod);
    // Destructor por defecto: unique_ptr se encarga del modelo
    ~proyectil() = default;

    // Actualiza posición usando el modelo físico
    void actualizar(float dt);

    // Verifica si debe eliminarse de la pantalla (fuera de un área razonable)
    bool debeDestruirse() const;

    // Getters para colisiones y renderizado
    float getX() const;
    float getY() const;
    int getDanio() const;
    float getVelX() const;
    float getVelY() const;
};

#endif
