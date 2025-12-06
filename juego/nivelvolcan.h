#ifndef NIVELVOLCAN_H
#define NIVELVOLCAN_H
#include <vector>

#include "nivel.h"
#include "proyectil.h"
#include "arma.h"
class NivelVolcan : public Nivel
{

private:

    Arma* armaVolcan;
    float tiempoAcumulado;
    float intervaloGeneracion;


    // Opcional: banderas si luego quieres terminar el nivel
    bool completado = false;
    bool fallido = false;
    // --- VIDAS (corazones que caen del cielo) ---
    std::vector<proyectil*> corazones;
    float tiempoCorazones = 0.0f;
    float intervaloCorazones = 5.0f; // cada 5 s cae un corazón


public:

    NivelVolcan();
    ~NivelVolcan() override;

    void generarRoca();
    void eliminarRocaEnIndice(int indice);
    void actualizar(float dt) override;


    float generarNumeroaleatorio(float min, float max);
    // Corazones (para que MainWindow los pueda dibujar)
    const std::vector<proyectil*>& getCorazones() const { return corazones; }
    void generarCorazonDesdeArriba();
    void actualizarCorazones(float dt);
    void eliminarCorazonEnIndice(int indice);
};

#endif // NIVELVOLCAN_H

