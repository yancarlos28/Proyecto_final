// snowman.h
#ifndef SNOWMAN_H
#define SNOWMAN_H

#include "enemigo.h"
#include <vector>

class Snowman : public enemigo {

private:
    float escalaTamano;
    int   faseAtaque;
    float temperaturaPercibida;
    std::vector<float> historialTemperaturas;

    float velocidadX;

public:
    Snowman();
    ~Snowman() override = default;

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

