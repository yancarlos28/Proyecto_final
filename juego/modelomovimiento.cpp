#include "modelomovimiento.h"
#include <cmath>

// ============================
// ModeloMovimiento
// ============================

ModeloMovimiento::ModeloMovimiento(double g_)
    : g(g_) {}

// Movimiento parabólico simple: aceleración solo en Y
void ModeloMovimiento::aplicar(float &x, float &y, float &vx, float &vy, float dt) const
{
    // 1. Actualizar velocidad vertical por gravedad
    vy += static_cast<float>(g) * dt;

    // 2. Actualizar posición
    x += vx * dt;
    y += vy * dt + 0.5 * static_cast<float>(g) * dt * dt;
}

// ============================
// MovimientoPendular
// ============================

MovimientoPendular::MovimientoPendular()
    : ax(0.0f), ay(0.0f),
    L(200.0f),
    amplitud(0.6f),
    velocidadAngular(3.0f), // ajusta para hacerlo más rápido
    tiempo(0.0f)
{
}

void MovimientoPendular::configurar(float xAncla, float yAncla, float longitud) {
    ax = xAncla;
    ay = yAncla;
    L  = (longitud > 0.0) ? longitud : 1.0;
}

// configurar amplitud (en radianes) y velocidad (rad/s)
void MovimientoPendular::configurarMovimiento(float nuevaAmplitud, float nuevaVelocidad) {
    amplitud         = nuevaAmplitud;
    velocidadAngular = nuevaVelocidad;
}

void MovimientoPendular::reiniciar() {
    tiempo = 0.0;
}

// Actualiza el tiempo y devuelve la posición del extremo
void MovimientoPendular::actualizar(float dt, float &xExtremo, float &yExtremo) {
    tiempo += dt;

    // ángulo en función del tiempo (no se detiene nunca)
    float theta = amplitud * std::sin(velocidadAngular * tiempo);

    xExtremo = ax + L * std::sin(theta);
    yExtremo = ay + L * std::cos(theta);
}

float MovimientoPendular::getXAncla() const { return ax; }
float MovimientoPendular::getYAncla() const { return ay; }
float MovimientoPendular::getLongitud() const { return L; }

