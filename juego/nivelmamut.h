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

    // Soga en pendulo
    MovimientoPendular movimientoSoga;
    float sogaXExtremo;
    float sogaYExtremo;
    bool jugadorColgado;
    // Caida después de la soga
    float velCaida;   // velocidad vertical de caída
    float pisoY;      // altura del piso en este nivel (centro del caverman)

    int municionJugador = 0;      // cuántas lanzas puede tirar el jugador
    float tiempoFlechas = 0.0;
    float intervaloFlechas = 3.0f; // cada 2 segundos cae una flecha
    float velocidadFlechaY = 10.0f;    // velocidad de caída

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
    bool estaColgado() const;
    float getSogaXAncla() const;
    float getSogaYAncla() const { return movimientoSoga.getYAncla(); }
    float getSogaXExtremo() const { return sogaXExtremo; }
    float getSogaYExtremo() const { return sogaYExtremo; }
    void intentarAgarrarSoga();
    void soltarSoga();

    //Flechas caidas del "cielo"
    void GenerarFlechaDesdeArriba();
    void actualizarFlechas(float dt);
    void recogerFlechaPorIndice(int indice);
    //Municion
    int getMunicionJugador() const;





};

#endif // NIVELMAMUT_H
