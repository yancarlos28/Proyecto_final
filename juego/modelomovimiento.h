#ifndef MODELOMOVIMIENTO_H
#define MODELOMOVIMIENTO_H

#include <cmath>


// Movimiento Parabólico (MRUA con gravedad)
class ModeloMovimiento {

/*    double g;
    double posY;
    double posX;
    double vel;
    double ang;
    double tiempo;
    double velx;
    double vely;*/
    double g;

public:

/*    ModeloMovimiento();
    ModeloMovimiento(double x, double y, double v, double a);
    void CalcularVelocidad();
    void CalcularPosicion();
    void ActualizarVelocidad();
    double getPosX() const;
    double getPosY() const;
*/
    ModeloMovimiento(double g = -9.8);
    void aplicar(float &x, float &y,
                 float &vx, float &vy,
                 float dt) const;
};


/*
// Movimiento Rectilíneo Uniforme (MRU)
class MovimientoRectilineo : public ModeloMovimiento {
public:
    MovimientoRectilineo() = default;

    void aplicar(float &x, float &y, float &vx, float &vy, float t) override {
        x += vx * t;
        y += vy * t;
    }

    ModeloMovimiento* clonar() const override {
        return new MovimientoRectilineo();
    }
};

// Movimiento Oscilatorio (Sinusoidal)
class MovimientoOscilatorio : public ModeloMovimiento {
private:
    float amplitud;
    float frecuencia;
    float tiempoAcumulado;
    float velocidadBaseX;  // Velocidad horizontal constante

public:
    MovimientoOscilatorio(float amp, float freq, float velX = 0)
        : amplitud(amp), frecuencia(freq), tiempoAcumulado(0), velocidadBaseX(velX) {}

    void aplicar(float &x, float &y, float &vx, float &vy, float t) override {
        tiempoAcumulado += t;

        // Movimiento horizontal (si existe)
        x += velocidadBaseX * t;

        // Movimiento oscilatorio en Y
        float desplazamientoY = amplitud * sin(frecuencia * tiempoAcumulado);
        y += desplazamientoY * dt * 10; // Ajuste de sensibilidad
    }

    ModeloMovimiento* clonar() const override {
        return new MovimientoOscilatorio(amplitud, frecuencia, velocidadBaseX);
    }
};
*/
#endif
