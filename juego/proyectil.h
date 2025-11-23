#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "modelomovimiento.h"
#include <memory>
using namespace std;

class proyectil {

private:

    float posX, posY;
    float velX, velY;
    int danio;

    // Smart pointer para seguridad
    unique_ptr<ModeloMovimiento> modelo;

public:
    //constructor
    proyectil(float x, float y, float vx, float vy, int dmg, ModeloMovimiento* mod);

    ~proyectil() = default; //Importante! El unique_ptr se libera solo

    // Actualiza posición usando el modelo físico
    void actualizar(float dt);

    // Verifica si debe eliminarse de la pantalla.
    bool debeDestruirse() const;

    // Getters para colisiones y renderizado
    float getX() const;
    float getY() const;
    int getDanio() const;
};

#endif
