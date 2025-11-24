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
    float velSaltoY;
    float pisoY;

public:

    caverman();
    ~caverman() override = default;

    // Getters para energía y puntuación
    int getEnergia() const;
    int getPuntuacion() const;

    void setEnergia(int val);
    void setPuntuacion(int val);

    void lanzar();
    void curar(int valor);
    void debugPrint() const override;

    //para salto
    bool estaSaltando() const;
    void iniciarSalto();
    void actualizarSalto(float dt);
};

#endif
