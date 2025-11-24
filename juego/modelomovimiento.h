#ifndef MODELOMOVIMIENTO_H
#define MODELOMOVIMIENTO_H

#include <cmath>


// Movimiento Parabólico
class ModeloMovimiento {

    double g;

public:

    ModeloMovimiento(double g = 500);
    void aplicar(float &x, float &y,float &vx, float &vy,float dt) const;
};

class MovimientoPendular {
private:
    float ax;      // x del ancla (techo)
    float ay;      // y del ancla
    float L;       // longitud de la soga

    float amplitud;        // amplitud del ángulo (rad)
    float velocidadAngular; // velocidad de oscilación (rad/s)

    float tiempo;  // tiempo acumulado

public:
    MovimientoPendular()
        : ax(0.0f), ay(0.0f),
        L(200.0f),
        amplitud(0.6f),        // ~34 grados
        velocidadAngular(3.0f),// ajusta para hacerlo más rápido
        tiempo(0.0f)
    {}

    void configurar(float xAncla, float yAncla, float longitud) {
        ax = xAncla;
        ay = yAncla;
        L  = (longitud > 0.0f) ? longitud : 1.0;
    }

    // configurar amplitud (en radianes) y velocidad (rad/s)
    void configurarMovimiento(float nuevaAmplitud, float nuevaVelocidad) {
        amplitud        = nuevaAmplitud;
        velocidadAngular = nuevaVelocidad;
    }

    void reiniciar() {
        tiempo = 0.0f;
    }

    // Actualiza el tiempo y devuelve la posición del extremo
    void actualizar(float dt, float &xExtremo, float &yExtremo) {
        tiempo += dt;

        // ángulo en función del tiempo (no se detiene nunca)
        float theta = amplitud * std::sin(velocidadAngular * tiempo);

        xExtremo = ax + L * std::sin(theta);
        yExtremo = ay + L * std::cos(theta);
    }

    float getXAncla() const { return ax; }
    float getYAncla() const { return ay; }
    float getLongitud() const { return L; }
};



#endif
