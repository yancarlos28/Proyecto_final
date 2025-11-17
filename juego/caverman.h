#ifndef CAVERMAN_H
#define CAVERMAN_H

#include "personaje.h"

class caverman : public personaje {
public:
    int energia;
    int puntuacion;

    caverman();
    ~caverman() override = default;

    void lanzar();
    void curar(int valor);

    void debugPrint() const override;
}
#endif // CAVERMAN_H
