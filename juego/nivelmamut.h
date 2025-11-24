#ifndef NIVELMAMUT_H
#define NIVELMAMUT_H

#include "nivel.h"
#include "mamut.h"
#include "arma.h"
#include "modelomovimiento.h"


class NivelMamut : public Nivel
{
private:
    mamut jefeMamut;
    float anchoMundo;
    Arma* armaJugador;

    // --- SOGA Y PÉNDULO ---
    MovimientoPendular movimientoSoga;
    float sogaXExtremo;
    float sogaYExtremo;
    bool jugadorColgado;
public:

    NivelMamut();
    ~NivelMamut() override;

    void actualizar(float dt) override;

    mamut& getMamut() { return jefeMamut; }
    const mamut& getMamut() const { return jefeMamut; }

    float getAnchoMundo() const { return anchoMundo; }

    // Lanzas disparadas por el jugador
    //const std::vector<proyectil*>& getLanzasJugador() const { return proyectilesJugador; }
    void lanzarDesdeJugador(float dirX);
    void eliminarLanzaEnIndice(int indice);

    // --- SOGA ---
    void actualizarSoga(float dt);
    bool estaColgado() const { return jugadorColgado; }
    float getSogaXAncla() const { return movimientoSoga.getXAncla(); }
    float getSogaYAncla() const { return movimientoSoga.getYAncla(); }
    float getSogaXExtremo() const { return sogaXExtremo; }
    float getSogaYExtremo() const { return sogaYExtremo; }

    void intentarAgarrarSoga();
    void soltarSoga();

};

#endif // NIVELMAMUT_H
