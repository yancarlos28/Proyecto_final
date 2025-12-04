#include "snowman.h"
#include <QDebug>
#include <cmath>

Snowman::Snowman()
    : escalaTamano(1.0f),
    faseAtaque(1),
    temperaturaPercibida(0.0f),
    velocidadX(200.0f)      // píxeles por segundo (ajusta si va muy rápido/lento)
{
    setPos(800, 300);   // posición inicial del jefe
    setVida(300);
    setAlerta(true);
}

void Snowman::actuar(float dt)
{
    // 1) Decidir fase según la temperatura
    //    Más frío -> fase más alta -> más poderoso.
    if (temperaturaPercibida > -5.0f) {
        faseAtaque = 1;   // Fase 1: quieto, dispara parabólico
    }
    else if (temperaturaPercibida > -15.0f) {
        faseAtaque = 2;   // Fase 2: se mueve + parabólico
    }
    else {
        faseAtaque = 3;   // Fase 3: se mueve + ondulatorio
    }

    // 2) Escala lógica (para que se vea más grande cuanto más frío)
    switch (faseAtaque) {
    case 1:  escalaTamano = 1.0f; break;
    case 2:  escalaTamano = 1.2f; break;
    case 3:  escalaTamano = 1.5f; break;
    }

    // 3) Movimiento horizontal SOLO en fases 2 y 3
    if (faseAtaque >= 2) {
        float x = getX();
        float y = getY();

        x += velocidadX * dt;

        // Límites simples de movimiento (ajusta a tu escena 1920x1200)
        const float limiteIzq = 200.0f;
        const float limiteDer = 1600.0f;

        if (x < limiteIzq) {
            x = limiteIzq;
            velocidadX = std::fabs(velocidadX);  // rebota hacia la derecha
        }
        else if (x > limiteDer) {
            x = limiteDer;
            velocidadX = -std::fabs(velocidadX); // rebota hacia la izquierda
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
    case 1:  return 0.8f;  // hace menos daño
    case 2:  return 1.2f;  // normal/fuerte
    case 3:  return 1.6f;  // muy fuerte
    default: return 1.0f;
    }
}

// Por ahora no lo usamos directamente (el nivel gestiona los disparos)
void Snowman::lanzarHielo()
{
    // Aquí podrías poner lógica futura del propio jefe si quieres.
}
