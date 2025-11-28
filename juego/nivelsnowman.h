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
// ==== NUEVO: antorcha del jugador y bolitas de fuego ====
struct AntorchaJugador {
    float x;
    float y;
    float velocidadY;
    bool activa;
    bool explotada;
    float angulo;      // en grados
    float velAngular;  // grados por segundo
    // NUEVO:
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

    // Acceso a los proyectiles para la interfaz
    const vector<proyectil*>& getBolasParabolicas() const;
    const vector<BolaNieveOscilante>& getBolasOscilantes() const;
    void lanzarAntorcha();  // llamado desde MainWindow (tecla)
    const AntorchaJugador& getAntorcha() const;
    bool hayAntorchaActiva() const;
    // ⬇⬇⬇ NUEVO: temperatura para el termómetro ⬇⬇⬇
    float getTemperaturaActual() const { return temperaturaActual; }


    const std::vector<BolitaFuego>& getBolitasFuego() const;
    void eliminarBolitaFuegoEnIndice(size_t i);

private:

    // Jefe
    Snowman jefeSnow;

    // Arma para bolas parabólicas
    Arma* armaParabolica;
    vector<proyectil*> bolasParabolicas;
    // ⬇⬇⬇ NUEVO: variables de temperatura ⬇⬇⬇
    float temperaturaActual;      // °C actual del nivel
    float tempMax;                // por ejemplo 0 °C
    float tempMin;                // por ejemplo -30 °C
    float velocidadEnfriamiento;  // °C/segundo
    // Bolas con movimiento oscilatorio
    vector<BolaNieveOscilante> bolasOscilantes;
    // --- NUEVO: sistema de antorcha del jugador ---
    AntorchaJugador antorcha;
    std::vector<BolitaFuego> bolitasFuego;
    float alturaExplosionAntorcha;   // y donde explota

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
    // --- NUEVO ---
    void actualizarAntorchaYFuego(float dt);
    void crearFragmentosDesdeAntorcha();


};

#endif // NIVELSNOWMAN_H
