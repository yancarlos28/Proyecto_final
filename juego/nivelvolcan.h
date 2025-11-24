#ifndef NIVELVOLCAN_H
#define NIVELVOLCAN_H
#include <vector>

#include "nivel.h"
#include "proyectil.h"
#include "arma.h"

class NivelVolcan : public Nivel
{

private:

    Arma* armaVolcan;
    float tiempoAcumulado;
    float intervaloGeneracion;


    // Opcional: banderas si luego quieres terminar el nivel
    bool completado = false;
    bool fallido = false;

public:

    NivelVolcan();
    ~NivelVolcan() override;

    void generarRoca();
    void actualizar(float dt) override;

    // Interfaz del nivel
    bool estaCompletado() const override { return completado; }
    bool estaFallido() const override { return fallido; }

};

#endif // NIVELVOLCAN_H

