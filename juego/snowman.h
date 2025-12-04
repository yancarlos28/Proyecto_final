// snowman.h
#ifndef SNOWMAN_H
#define SNOWMAN_H

#include "enemigo.h"
#include <vector>

class Snowman : public enemigo {

private:
    float escalaTamano;          // factor lógico de escala (1, 1.2, 1.5, ...)
    int   faseAtaque;            // 1, 2, 3
    float temperaturaPercibida;  // °C
    std::vector<float> historialTemperaturas;

    float velocidadX;            // velocidad horizontal para fases 2 y 3

public:
    Snowman();
    ~Snowman() override = default;

    void lanzarHielo();                 // por ahora vacío, lo maneja el nivel
    void actuar(float dt) override;
    void debugPrint() const override;

    // Interfaces del agente inteligente
    void percibirTemperatura(float temp);
    void aprenderDeTemperatura(float temp);

    // Getters para el nivel/UI
    int   getFaseAtaque()   const { return faseAtaque; }
    float getEscalaTamano() const { return escalaTamano; }

    // Multiplicador de daño según fase
    float getMultiplicadorDanio() const;
};

#endif

