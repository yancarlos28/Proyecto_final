#ifndef MODELOMOVIMIENTO_H
#define MODELOMOVIMIENTO_H

// Movimiento Parabólico
class ModeloMovimiento {

    double g;

public:
    explicit ModeloMovimiento(double g = 500);
    void aplicar(float &x, float &y, float &vx, float &vy, float dt) const;
};

// Movimiento Pendular
class MovimientoPendular {
private:
    float ax;      // x del ancla
    float ay;      // y del ancla
    float L;       // longitud de la soga

    float amplitud;         // amplitud del ángulo (rad)
    float velocidadAngular; // velocidad de oscilación (rad/s)

    float tiempo;  // tiempo acumulado

public:
    MovimientoPendular();

    void configurar(float xAncla, float yAncla, float longitud);

    // configurar amplitud (en radianes) y velocidad (rad/s)
    void configurarMovimiento(float nuevaAmplitud, float nuevaVelocidad);

    void reiniciar();

    // Actualiza el tiempo y devuelve la posición del extremo
    void actualizar(float dt, float &xExtremo, float &yExtremo);

    float getXAncla() const;
    float getYAncla() const;
    float getLongitud() const;
};

#endif // MODELOMOVIMIENTO_H

