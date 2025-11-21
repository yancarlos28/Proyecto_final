#include "nivelvolcan.h"

// Constructor: estado inicial del nivel volcán
NivelVolcan::NivelVolcan()
{
    // Posición inicial del caverman en el nivel
    // Ajusta estos valores según cómo se vea en tu escena
    jugador.setPos(10, 40);
}

// Actualización lógica del nivel
void NivelVolcan::actualizar(float dt)
{
    // Por ahora no hacemos nada.
    // Más adelante aquí meterás:
    // - rocas que caen
    // - daño al jugador
    // - condiciones de victoria/derrota
    (void)dt; // para evitar warning mientras no lo usamos
}
