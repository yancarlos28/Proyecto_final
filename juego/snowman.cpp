#include "snowman.h"
#include <QDebug>
#include <cmath>

Snowman::Snowman()
    : escalaTamano(1.0),
    faseAtaque(1),
    temperaturaPercibida(0.0),
    velocidadX(200.0)      // píxeles por segundo
{
    setPos(800, 300);   // posición inicial del jefe Snowman
    setVida(300);       //Vida
    setAlerta(true);
}

void Snowman::actuar(float dt)
{
    // 1) Decidir fase según la temperatura
    if (temperaturaPercibida > -5.0) {
        faseAtaque = 1;
    }
    else if (temperaturaPercibida > -15.0f) {
        faseAtaque = 2;
    }
    else {
        faseAtaque = 3;
    }

    // 2) Escala lógica del tamaño
    switch (faseAtaque) {
    case 1:  escalaTamano = 1.0; break;
    case 2:  escalaTamano = 1.2; break;
    case 3:  escalaTamano = 1.5; break;
    }

    // 3) Movimiento horizontal solo en fases 2 y 3
    if (faseAtaque >= 2) {
        float x = getX();
        float y = getY();

        x += velocidadX * dt;

        // Límites simples de movimiento (ajusta a tu escena 1920x1200)
        const float limiteIzq = 200.0;
        const float limiteDer = 1600.0;

        if (x < limiteIzq) {
            x = limiteIzq;
            velocidadX = std::fabs(velocidadX);  // rebotar hacia la derecha
        }
        else if (x > limiteDer) {
            x = limiteDer;
            velocidadX = -std::fabs(velocidadX); // rebotar hacia la izquierda
        }

        setPos(x, y);
    }

    // 4) “Aprender”: guardar la temperatura que ha visto
    aprenderDeTemperatura(temperaturaPercibida);
}

void Snowman::debugPrint() const
{
    qDebug() << "Snowman pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()
    << " temp=" << temperaturaPercibida
    << " fase=" << faseAtaque
    << " escala=" << escalaTamano;
}

void Snowman::percibirTemperatura(float temp)
{
    temperaturaPercibida = temp;
}

void Snowman::aprenderDeTemperatura(float temp)
{
    historialTemperaturas.push_back(temp);
}

float Snowman::getMultiplicadorDanio() const
{
    // Cuanto más alta la fase, más daño hacen sus ataques
    switch (faseAtaque) {
    case 1:  return 0.8;  // hace menos daño
    case 2:  return 1.2;  // normal/fuerte
    case 3:  return 1.6;  // muy fuerte
    default: return 1.0;
    }
}

