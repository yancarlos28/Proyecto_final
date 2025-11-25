#ifndef NIVELSNOWMAN_H
#define NIVELSNOWMAN_H

#include "nivel.h"
#include "snowman.h"
#include "arma.h"
#include <vector>

using std::vector;

// Bola de nieve con movimiento oscilatorio
struct BolaNieveOscilante {
    float x;
    float y;
    float velocidadX;   // hacia izquierda o derecha
    float velocidadY;   //
    float amplitud;     // cuánto se mueve en Y
    float fase;         // ángulo actual de la sinusoide
    float frecuencia;   // velocidad angular de la oscilación
    float yCentro;      // línea base de la oscilación
};

class NivelSnowman : public Nivel
{
public:
    NivelSnowman();
    ~NivelSnowman();
    void eliminarBolaPorIndice(size_t i);

    void actualizar(float dt) override;

    // Acceso al jefe para la interfaz
    Snowman& getSnowman();
    const Snowman& getSnowman() const;

    // Acceso a los proyectiles para la interfaz
    const vector<proyectil*>& getBolasParabolicas() const;
    const vector<BolaNieveOscilante>& getBolasOscilantes() const;

private:

    // Jefe
    Snowman jefeSnow;

    // Arma para bolas parabólicas
    Arma* armaParabolica;
    vector<proyectil*> bolasParabolicas;

    // Bolas con movimiento oscilatorio
    vector<BolaNieveOscilante> bolasOscilantes;

    // Control de patrones de ataque
    enum class PatronAtaque { Parabolico, Oscilatorio };
    PatronAtaque patronActual;

    float tiempoDesdeUltimoDisparo;
    float intervaloDisparo;      // cada cuánto dispara en el patrón actual

    float tiempoCambioPatron;
    float intervaloCambioPatron; // cada cuánto cambia de patrón (A/B)

    // Lógica interna
    void dispararParabolicoAlJugador();
    void dispararRafagaOscilante();

    void actualizarBolasParabolicas(float dt);
    void actualizarBolasOscilantes(float dt);

    //void eliminarBolaPorIndice(size_t i);
};

#endif // NIVELSNOWMAN_H
