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
    vector<float> ruta = {400, 600, 1000, 1400};
    jefeMamut.setRuta(ruta);

    //Modelo de movimiento de la lanza y tiempo que se genera
    ModeloMovimiento* modeloLanza = new ModeloMovimiento(700);
    armaJugador = new Arma(20, 800, modeloLanza);
    municionJugador = 0;
    tiempoFlechas = 0.0;
    intervaloFlechas=3.0;

    // --- Configurar soga ---
    float xAncla = 800.0;
    float yAncla = 20.0;
    float longitud =600.0;

    //Movimiento de la soga en "péndulo"
    movimientoSoga.configurar(xAncla, yAncla, longitud);

    // amplitud en radianes, velocidad en rad/s
    movimientoSoga.configurarMovimiento(1.0, 3.0);

    // posición inicial del extremo
    sogaXExtremo = xAncla;
    sogaYExtremo = yAncla + longitud;

    jugadorColgado = false;

    // La caída del caverman en la cuerda
    pisoY = jugador.getY();
    velCaida = 0.0;

}
//Destructor
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

    // Si no hay munición, NO lanza
    if (municionJugador <= 0) {
        qDebug() << "[Mamut] Sin lanzas, recoge flechas!";
        return;
    }

    // consumimos una lanza
    municionJugador--;

    // Normalizar signo
    float dx = (dirX >= 0.0f) ? 1.0 : -1.0;
    float dy = -1;

    float x = jugador.getX();
    float y = jugador.getY() - 30.0f;

    proyectil* p = armaJugador->crearProyectil(x, y, dx, dy);
    proyectilesJugador.push_back(p);
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
void NivelMamut::recogerFlechaPorIndice(int indice)
{
    if (indice < 0 || indice >= static_cast<int>(proyectilesEnemigos.size()))
        return;

    proyectil* p = proyectilesEnemigos[indice];
    if (!p) return;

    // Aumentar munición del jugador
    municionJugador+=3;
    qDebug() << "[Mamut] Flecha recogida. Munición =" << municionJugador;

    delete p;
    proyectilesEnemigos.erase(proyectilesEnemigos.begin() + indice);
}

void NivelMamut::actualizarFlechas(float dt)
{
    int i = 0;
    while (i < static_cast<int>(proyectilesEnemigos.size())) {
        proyectil* p = proyectilesEnemigos[i];
        if (!p) { ++i; continue; }

        p->actualizar(dt);

        // si sale de la pantalla, la destruimos
        if (p->debeDestruirse()) {
            delete p;
            proyectilesEnemigos.erase(proyectilesEnemigos.begin() + i);
        } else {
            ++i;
        }
    }
}

//Actualizar nivel/soga/flechas
void NivelMamut::actualizar(float dt)
{
    // 1) Movimiento / IA del mamut
    jefeMamut.actuar(dt);

    // 2) Soga
    actualizarSoga(dt);

    // 3) Movimiento del jugador (caída)
    if (!jugadorColgado) {
        float x = jugador.getX();
        float y = jugador.getY();

        if (y < pisoY) {
            float g = 800.0f;
            velCaida += g * dt;
            y += velCaida * dt;

            if (y >= pisoY) {
                y = pisoY;
                velCaida = 0.0f;
            }

            jugador.setPos(x, y);
        } else {
            velCaida = 0.0f;
            jugador.setPos(x, pisoY);
        }
    }

    // 4) Actualizar lanzas del jugador
    for (proyectil* p : proyectilesJugador) {
        p->actualizar(dt);
    }

    // 5) Generar flechas periódicamente
    tiempoFlechas += dt;
    if (tiempoFlechas >= 2.0) {   // o intervaloFlechas si lo usas
        tiempoFlechas = 0.0;
        GenerarFlechaDesdeArriba();
    }

    // 6) Actualizar flechas enemigas (las que caen)
    actualizarFlechas(dt);

}


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
void NivelMamut::actualizarSoga(float dt)
{
    movimientoSoga.actualizar(dt, sogaXExtremo, sogaYExtremo);

    if (jugadorColgado) {
        jugador.setPos(sogaXExtremo, sogaYExtremo);
    }
}

bool NivelMamut::estaColgado() const { return jugadorColgado; }
float NivelMamut::getSogaXAncla() const { return movimientoSoga.getXAncla(); }
int NivelMamut::getMunicionJugador() const { return municionJugador; }
