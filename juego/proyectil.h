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
    unique_ptr<ModeloMovimiento> modelo; // Smart pointer para seguridad

public:
    proyectil(float x, float y, float vx, float vy, int dmg, ModeloMovimiento* mod);
    ~proyectil() = default; // El unique_ptr se libera solo

    // Actualiza posición usando el modelo físico
    void actualizar(float dt);

    // Verifica si debe eliminarse (fuera de pantalla)
    bool debeDestruirse() const;

    // Getters para colisiones y renderizado
    float getX() const { return posX; }
    float getY() const { return posY; }
    int getDanio() const { return danio; }
};

#endif
