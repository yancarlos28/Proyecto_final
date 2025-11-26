#include "nivelmamut.h"
#include <QDebug>
NivelMamut::NivelMamut()
{
    // Posición inicial del jugador
    jugador.setPos(100, 800);

    // Posición inicial del mamut
    jefeMamut.setPos(700, 900);

    // Mundo más ancho para que haya scroll
    anchoMundo = 3000;

    // Ruta específica del mamut a lo largo del nivel
    vector<float> ruta = { 400, 600, 1000, 1400};
    jefeMamut.setRuta(ruta);

    ModeloMovimiento* modeloLanza = new ModeloMovimiento(340);
    armaJugador = new Arma(20, 700, modeloLanza);


    // --- Configurar soga ---
    float xAncla = 800.0;
    float yAncla = 20.0;
    float longitud =600.0;

    movimientoSoga.configurar(xAncla, yAncla, longitud);

    // amplitud en radianes, velocidad en rad/s
    movimientoSoga.configurarMovimiento(1.0, 3.0);

    // posición inicial del extremo
    sogaXExtremo = xAncla;
    sogaYExtremo = yAncla + longitud;

    jugadorColgado = false;

    // --- CAÍDA ---
    pisoY = jugador.getY();
    velCaida = 0.0f;

    municionJugador = 0;
    tiempoFlechas = 0.0;
}

NivelMamut::~NivelMamut()
{
    /*for (proyectil* p : proyectilesJugador) {
        delete p;
    }
    proyectilesJugador.clear();*/

    delete armaJugador; //Nivel ya lo hace
}
//Lanza
void NivelMamut::lanzarDesdeJugador(float dirX)
{
    if (!armaJugador) return;
    // Si no hay munición, no lanza
    /*if (municionJugador <= 0) {
        qDebug() << "[Mamut] Sin lanzas, recoge flechas!";
        return;
    }*/
    // Normalizar signo
    float dx = (dirX >= 0.0) ? 1.0 : -1.0;
    // un poco hacia arriba
    float dy = -0.6;

    float x = jugador.getX();
    //salga del jugador
    float y = jugador.getY() - 30.;

    proyectil* p = armaJugador->crearProyectil(x, y, dx, dy);
    proyectilesJugador.push_back(p);
    municionJugador--;
}
void NivelMamut::GenerarFlechaDesdeArriba()
{
    if (!armaJugador) return;

    // X aleatoria dentro del mundo (ajusta a tu fondo)
    float xMin = 200.0;
    float xMax = 1400.0;
    float x = xMin + static_cast<float>(rand()) / RAND_MAX * (xMax - xMin);

    float y = 0.0;   // arriba del todo

    float dx = 0.0;
    float dy = 1.0;

    proyectil* p = armaJugador->crearProyectil(x, y, dx, dy);

    proyectilesEnemigos.push_back(p);
}

void NivelMamut::eliminarLanzaEnIndice(int indice)
{

    if (indice < 0 || indice >= static_cast<int>(proyectilesJugador.size())) {
        return;
    }

    delete proyectilesJugador[indice];
    proyectilesJugador.erase(proyectilesJugador.begin() + indice);

}

//Actualizar
void NivelMamut::actualizar(float dt)
{
    // 1) Movimiento / IA del mamut
    jefeMamut.actuar(dt);

    // 2) Actualizar soga (se mueve sola siempre)
    actualizarSoga(dt);

    // 3) Movimiento del jugador
    if (!jugadorColgado) {
        float x = jugador.getX();
        float y = jugador.getY();

        // Si está por encima del piso, aplicamos gravedad
        if (y < pisoY) {
            float g = 800.0f;  // misma gravedad que usas en el salto
            velCaida += g * dt;
            y += velCaida * dt;

            if (y >= pisoY) {
                y = pisoY;
                velCaida = 0.0f; // llegó al suelo
            }

            jugador.setPos(x, y);
        } else {
            // Aseguramos que se quede pisando el suelo
            velCaida = 0.0f;
            jugador.setPos(x, pisoY);
        }
    }

    // 4) Actualizar lanzas del jugador
    for (proyectil* p : proyectilesJugador) {
        p->actualizar(dt);
    }

}
void NivelMamut::actualizarSoga(float dt)
{
    movimientoSoga.actualizar(dt, sogaXExtremo, sogaYExtremo);

    if (jugadorColgado) {
        jugador.setPos(sogaXExtremo, sogaYExtremo);
    }
}
/*void NivelMamut::actualizarFlechas(float dt)
{
    for (proyectil* p : proyectilesEnemigos) {
        if (!p) continue;

        // ya se están moviendo con su propio modelo:
        p->actualizar(dt);

        // Opcional: si quieres que se queden "clavadas" en el suelo
        float y = p->getY();
        if (y > pisoY) {
            p->setPos(p->getX(), pisoY);
            // aquí podrías frenar su movimiento, según cómo esté implementado proyectil
        }
    }
}*/

//Agarrar y soltar la soga
void NivelMamut::intentarAgarrarSoga()
{
    if (jugadorColgado) return;

    float xJug = jugador.getX();
    float yJug = jugador.getY();

    // Distancia entre jugador y extremo actual de la soga
    float dx = xJug - sogaXExtremo;
    float dy = yJug - sogaYExtremo;
    float dist2 = dx*dx + dy*dy;

    // distancia máxima para poder agarrarse
    float radio = 400.0;

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

    // Empezar caída desde donde está
    velCaida = 0.0f;
}
