#include "modelomovimiento.h"
#include <cmath>
/*
double ModeloMovimiento::getPosX() const
{
    return posX;
}

double ModeloMovimiento::getPosY() const
{
    return posY;
}

ModeloMovimiento::ModeloMovimiento(double x, double y, double v, double a)
{
    tiempo=0.01;
    g=-9.8;
    posX=x;
    posY=y;
    vel=v;
    ang=a;
}

void ModeloMovimiento::CalcularVelocidad()
{
    velx=vel*cos(ang);
    vely=vel*sin(ang)+g*tiempo;
}

void ModeloMovimiento::CalcularPosicion()
{
    posX+= velx*tiempo;
    posY+= vely*tiempo+(0.5*g*tiempo*tiempo);
}

void ModeloMovimiento::ActualizarVelocidad()
{
    vel=sqrt(pow(velx,2)+pow(vely,2));
    ang=atan2(vely,velx);
}
*/

ModeloMovimiento::ModeloMovimiento(double g_)
    : g(g_) {}

// Movimiento parabólico simple: aceleración solo en Y
void ModeloMovimiento::aplicar(float &x, float &y,float &vx, float &vy,float dt) const
{
    // 1. Actualizar velocidad vertical por gravedad
    vy += g * dt;

    // 2. Actualizar posición
    x += vx * dt;
    y += vy * dt + 0.5f * g * dt * dt;
}

