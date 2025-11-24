#include "nivelmamut.h"
#include <QDebug>
NivelMamut::NivelMamut()
{
    // Posición inicial del jugador
    jugador.setPos(100, 800);

    // Posición inicial del mamut
    jefeMamut.setPos(700, 800);

    // Mundo más ancho para que haya scroll
    anchoMundo = 3000;

    // Ruta específica del mamut a lo largo del nivel
    vector<float> ruta = { 200, 600, 1000, 1400};
    jefeMamut.setRuta(ruta);

    ModeloMovimiento* modeloLanza = new ModeloMovimiento(200);
    armaJugador = new Arma(10, 500, modeloLanza);

    // --- Configurar soga ---
    float xAncla = 800.0f;
    float yAncla = 100.0f;
    float longitud = 700.0f;

    movimientoSoga.configurar(xAncla, yAncla, longitud);

    // 🔹 AQUÍ SETEAS VELOCIDAD Y AMPLITUD
    // amplitud en radianes (0.6 ~ 34°), velocidad en rad/s
    movimientoSoga.configurarMovimiento(0.8f, 2.0f); // 4.0f => rápido

    // posición inicial del extremo
    sogaXExtremo = xAncla;
    sogaYExtremo = yAncla + longitud;

    jugadorColgado = false;


}

NivelMamut::~NivelMamut()
{
    for (proyectil* p : proyectilesJugador) {
        delete p;
    }
    proyectilesJugador.clear();

    delete armaJugador;
}
void NivelMamut::lanzarDesdeJugador(float dirX)
{
    if (!armaJugador) return;

    // Normalizar signo
    float dx = (dirX >= 0.0) ? 1.0 : -1.0;
    float dy = -0.3;   // un poco hacia arriba

    float x = jugador.getX();
    float y = jugador.getY() - 30.; //salga del jugador

    proyectil* p = armaJugador->crearProyectil(x, y, dx, dy);
    proyectilesJugador.push_back(p);
}

void NivelMamut::eliminarLanzaEnIndice(int indice)
{

    if (indice < 0 || indice >= static_cast<int>(proyectilesJugador.size())) {
        return;
    }

    delete proyectilesJugador[indice];
    proyectilesJugador.erase(proyectilesJugador.begin() + indice);

}

void NivelMamut::actualizar(float dt)
{
    // Movimiento / IA del mamut
    jefeMamut.actuar(dt);
    // 2) Actualizar soga (se mueve sola siempre)
    actualizarSoga(dt);
    // Actualizar lanzas del jugador
    for (proyectil* p : proyectilesJugador) {
        p->actualizar(dt);
    }
    // Si quieres, aquí puedes limpiar lanzas que salen de pantalla
    /*
    int j = 0;
    while (j < static_cast<int>(proyectilesJugador.size())) {
        float x = proyectilesJugador[j]->getX();
        float y = proyectilesJugador[j]->getY();

        if (y > 720.0f || x < 0.0f || x > anchoMundo) {
            delete proyectilesJugador[j];
            proyectilesJugador.erase(proyectilesJugador.begin() + j);
        } else {
            ++j;
        }
    }
    */
}
void NivelMamut::actualizarSoga(float dt)
{
    movimientoSoga.actualizar(dt, sogaXExtremo, sogaYExtremo);

    if (jugadorColgado) {
        jugador.setPos(sogaXExtremo, sogaYExtremo);
    }
}

void NivelMamut::intentarAgarrarSoga()
{
    if (jugadorColgado) return;

    float xJug = jugador.getX();
    float yJug = jugador.getY();

    // Distancia entre jugador y extremo actual de la soga
    float dx = xJug - sogaXExtremo;
    float dy = yJug - sogaYExtremo;
    float dist2 = dx*dx + dy*dy;

    float radio = 60.0f; // distancia máxima para poder agarrarse

    if (dist2 < radio * radio) {
        jugadorColgado = true;

        // Lo pegamos exacto al extremo
        jugador.setPos(sogaXExtremo, sogaYExtremo);
    }
}
void NivelMamut::soltarSoga()
{
    if (!jugadorColgado) return;

    jugadorColgado = false;

    // Opcional: podrías darle un "impulso" según la velocidad del péndulo
    // De momento lo dejamos caer con su física normal
}
