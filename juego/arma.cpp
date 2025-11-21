#include "arma.h"
#include <QDebug>
#include <cmath>

Arma::Arma(int dmg, float vel, ModeloMovimiento* model)
    : danioBase(dmg), velocidadProyectil(vel), modelo(model)
{}

Arma::~Arma() {
    delete modelo; // El arma es dueña del modelo base
    qDebug() << "[Arma] Modelo base destruido";
}

// Crear proyectil con dirección (dirX, dirY)
proyectil* Arma::crearProyectil(float x, float y, float dirX, float dirY)
{
    // 1. Calcular la magnitud de la dirección
    float magnitud = std::sqrt(dirX * dirX + dirY * dirY);
    if (magnitud < 0.001f) magnitud = 1.0f;

    // 2. Normalizar el vector de dirección
    float dx = dirX / magnitud;
    float dy = dirY / magnitud;

    // 3. Velocidades iniciales
    float vx = dx * velocidadProyectil;
    float vy = dy * velocidadProyectil;

    // 4. Clonar el modelo base (solo gravedad)
    ModeloMovimiento* modeloClonado = new ModeloMovimiento(*modelo);

    qDebug() << "[Arma] Proyectil creado en"
             << x << "," << y << "vel:"
             << vx << "," << vy;

    // 5. Crear proyectil nuevo
    return new proyectil(x, y, vx, vy, danioBase, modeloClonado);
}


