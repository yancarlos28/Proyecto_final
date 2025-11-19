// mamut.h
#ifndef MAMUT_H
#define MAMUT_H

#include "enemigo.h"
#include <vector>

class mamut : public enemigo {
private:
    std::vector<float> rutaPatrulla; // Puntos de patrullaje

public:
    mamut();
    ~mamut() override = default;

    void atacarAlJugador();
    void actuar(float dt) override;
    void debugPrint() const override;
};

#endif
