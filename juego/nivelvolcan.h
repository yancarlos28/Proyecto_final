#ifndef NIVELVOLCAN_H
#define NIVELVOLCAN_H

#include "nivel.h"

// Nivel del volcán (Nivel 2 del juego)
class NivelVolcan : public Nivel
{
public:
    NivelVolcan();

    // Actualiza la lógica del nivel (por ahora vacío)
    void actualizar(float dt) override;
};

#endif // NIVELVOLCAN_H

