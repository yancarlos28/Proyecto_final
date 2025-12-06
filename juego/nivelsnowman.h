#ifndef NIVELSNOWMAN_H
#define NIVELSNOWMAN_H

#include "nivel.h"
#include "snowman.h"
#include "arma.h"
#include <vector>

using std::vector;

struct BolaNieveOscilante {
    float x;
    float y;
    float velocidadX;
    float velocidadY;
    float amplitud;
    float fase;
    float frecuencia;
     // línea base de la oscilación
    float yCentro;

};
// Antorcha del jugador y bolitas de fuego
struct AntorchaJugador {
    float x;
    float y;
    float velocidadY;
    bool activa;
    bool explotada;
    float angulo;      // en °
    float velAngular;  // °/s
    float yInicio;             // y desde donde salió
    float distanciaExplosion;  // cuánto debe subir antes de explotar

};

struct BolitaFuego {
    float x;
    float y;
    float velX;
    float velY;
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

    // Acceso a los proyectiles para manejar en UI
    const vector<proyectil*>& getBolasParabolicas() const;
    const vector<BolaNieveOscilante>& getBolasOscilantes() const;
    const std::vector<BolitaFuego>& getBolitasFuego() const;

    const AntorchaJugador& getAntorcha() const;

    bool hayAntorchaActiva() const;
    float getTemperaturaActual() const;
    void eliminarBolitaFuegoEnIndice(size_t i);
    void lanzarAntorcha();
    // Lógica interna
    void dispararParabolicoAlJugador();
    void dispararRafagaOscilante();

    void actualizarBolasParabolicas(float dt);
    void actualizarBolasOscilantes(float dt);
    //
    void actualizarAntorchaYFuego(float dt);
    void crearFragmentosDesdeAntorcha();


private:

    // Jefe
    Snowman jefeSnow;

    // Arma para bolas parabólicas
    Arma* armaParabolica;
    vector<proyectil*> bolasParabolicas;

    // Variables de temperatura
    float temperaturaActual;
    float tempMax;
    float tempMin;
    float velocidadEnfriamiento;

    // Bolas con movimiento oscilatorio
    vector<BolaNieveOscilante> bolasOscilantes;
    // Sistema de antorcha del jugador
    AntorchaJugador antorcha;
    std::vector<BolitaFuego> bolitasFuego;
    float alturaExplosionAntorcha;

    // Control de patrones de ataque
    enum class PatronAtaque { Parabolico, Oscilatorio };
    PatronAtaque patronActual;

    float tiempoDesdeUltimoDisparo;
    float intervaloDisparo;

    float tiempoCambioPatron;



};

#endif // NIVELSNOWMAN_H
