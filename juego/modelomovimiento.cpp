#include "modelomovimiento.h"
#include <cmath>


ModeloMovimiento::ModeloMovimiento(double g_)
    : g(g_) {}

// Movimiento parabólico simple: aceleración solo en Y
void ModeloMovimiento::aplicar(float &x, float &y,float &vx, float &vy,float dt) const
{
    // 1. Actualizar velocidad vertical por gravedad
    vy += g * dt;

    // 2. Actualizar posición
    x += vx * dt;
    y += vy * dt + 0.5 * g * dt * dt;
}

