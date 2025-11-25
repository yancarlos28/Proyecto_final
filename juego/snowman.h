// snowman.h
#ifndef SNOWMAN_H
#define SNOWMAN_H

#include "enemigo.h"
#include <vector>

class Snowman : public enemigo {

private:
    float escalaTamano;
    int faseAtaque;
    float temperaturaPercibida;
    std::vector<float> historialTemperaturas; // Para aprendizaje

protected:
    // Métodos de razonamiento
    void decidirSiguienteAccion();

public:
    Snowman();
    ~Snowman() override = default;

    void lanzarHielo();
    void actuar(float dt) override;
    void debugPrint() const override;

    // Interfaces del agente inteligente
    void percibirTemperatura(float temp);
    void aprenderDeTemperatura(float temp);
};

#endif
