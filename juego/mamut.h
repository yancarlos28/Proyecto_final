// mamut.h
#ifndef MAMUT_H
#define MAMUT_H
#include "enemigo.h"
#include <vector>

using namespace std;

class mamut : public enemigo {
private:
    vector<float> rutaPatrulla;
    int indiceObjetivo;
    float velocidad;

public:

    mamut();
    ~mamut() override = default;

    void setRuta(const std::vector<float>& ruta);

    void atacarAlJugador();
    void actuar(float dt) override;
    void debugPrint() const override;
};

#endif
