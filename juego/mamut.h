// mamut.h
#ifndef MAMUT_H
#define MAMUT_H
#include "enemigo.h"
#include <vector>

using namespace std;

class mamut : public enemigo {
private:
    vector<float> rutaPatrulla; // Puntos de patrullaje en X
    int indiceObjetivo;              // Punto de la ruta al que va
    float velocidad;                 // pixeles por segundo
public:
    mamut();
    ~mamut() override = default;

    void setRuta(const std::vector<float>& ruta);

    void atacarAlJugador();
    void actuar(float dt) override;
    void debugPrint() const override;
};

#endif
