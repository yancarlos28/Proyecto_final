#ifndef NIVELVOLCAN_H
#define NIVELVOLCAN_H
#include <vector>
#include <memory>
#include "nivel.h"
#include "proyectil.h"
#include "arma.h"

class NivelVolcan : public Nivel
{
private:
    vector<proyectil*> rocas;
    Arma* armaVolcan;
    float tiempoAcumulado;
    float intervaloGeneracion;

    void generarRoca();
public:
    NivelVolcan();
    ~NivelVolcan();

    void actualizar(float dt) override;
    // acceso solo lectura a las rocas, para la GUI
    const vector<proyectil*>& getRocas() const { return rocas; }

};

#endif // NIVELVOLCAN_H

