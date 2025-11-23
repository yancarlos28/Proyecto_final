#include "nivelmamut.h"

NivelMamut::NivelMamut()
{
    // Posición inicial del jugador (mismo suelo que en volcán)
    jugador.setPos(100, 720);

    // Posición inicial del mamut
    jefeMamut.setPos(700, 720);

    // Mundo más ancho para que haya scroll
    anchoMundo = 3000;

    // Ruta específica del mamut a lo largo del nivel
    std::vector<float> ruta = { 200, 600, 1000, 1400};
    jefeMamut.setRuta(ruta);
}

void NivelMamut::actualizar(float dt)
{
    // Por ahora solo movemos al mamut según su IA simple
    jefeMamut.actuar(dt);

    // Más adelante puedes agregar colisiones caverman-mamut, etc.
}
