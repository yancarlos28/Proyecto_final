#ifndef MAMUT_H
#define MAMUT_H
#include "enemigo.h"
#include <vector>

class mamut : public enemigo {
public:
    std::vector<float> rutaPatrulla;

    mamut();
    ~mamut() override = default;

    void atacarAlJugador();
    void actuar(float dt) override;

    void debugPrint() const override;
};

#endif // MAMUT_H
