#include "proyectil.h"



proyectil::proyectil(float x, float y, float vx, float vy, int dmg, ModeloMovimiento* mod)
    : posX(x), posY(y), velX(vx), velY(vy), danio(dmg), modelo(mod) {}

void proyectil::actualizar(float dt) {
    if (modelo) {
        modelo->aplicar(posX, posY, velX, velY, dt);
    }
}
bool proyectil::debeDestruirse() const
{
    // Límites genéricos del “mundo” (los puedes ajustar fácil)
    const float LIM_X_MIN = -200.0f;
    const float LIM_X_MAX = 4000.0f;
    const float LIM_Y_MIN = -200.0f;
    const float LIM_Y_MAX = 2000.0f;

    return (posX < LIM_X_MIN || posX > LIM_X_MAX ||
            posY < LIM_Y_MIN || posY > LIM_Y_MAX);
}
//getters
float proyectil::getX() const
{
    return posX; }

float proyectil::getY() const
{
    return posY; }
float proyectil::getVelX() const
{
    return velX;
}

float proyectil::getVelY() const
{
    return velY;
}

int proyectil::getDanio() const {
    return danio; }
