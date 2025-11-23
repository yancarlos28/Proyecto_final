// caverman.h
#ifndef CAVERMAN_H
#define CAVERMAN_H

#include "personaje.h"
#include <algorithm>

class caverman : public personaje {

private:
    int energia;
    int puntuacion;

    // salto
    bool enSalto;
    float velSaltoY;   // velocidad vertical del salto

public:
    caverman();
    ~caverman() override = default;

    // Getters para energía y puntuación
    int getEnergia() const { return energia; }
    int getPuntuacion() const { return puntuacion; }

    void setEnergia(int val) { energia = std::min(100, val); }
    void setPuntuacion(int val) { puntuacion = val; }

    void lanzar();
    void curar(int valor);
    void debugPrint() const override;

    //para salto
    bool estaSaltando() const { return enSalto; }
    void iniciarSalto();
    void actualizarSalto(float dt);
};

#endif
