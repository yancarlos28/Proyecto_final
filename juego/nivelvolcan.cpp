#include "nivelvolcan.h"
#include "modelomovimiento.h"
#include <QDebug>

float NivelVolcan::generarNumeroaleatorio(float min, float max){
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float numero= min + r *(max-min);
    return numero;
}
NivelVolcan::NivelVolcan()
{
    // Posición inicial del caverman
    jugador.setPos(50, 720);

    //Rocas volcan
    tiempoAcumulado = 0;
    intervaloGeneracion = 0.5;

    // Modelo de movimiento parabólico
    ModeloMovimiento* modelo = new ModeloMovimiento(700);

    // Arma del volcán: daño , velocidad, modelo
    armaVolcan = new Arma(10, 600, modelo);

    // Corazones
    tiempoCorazones   = 0.0;
    intervaloCorazones = 6.0;

}
NivelVolcan::~NivelVolcan()
{
    // Borrar todas las rocas (proyectiles enemigos)
        for (proyectil* p : proyectilesEnemigos) {
        delete p;
    }
    proyectilesEnemigos.clear();

    delete armaVolcan;
}

void NivelVolcan::generarRoca()
{
    float origenX = 1090;
    float origenY = 210;

    // Variaciones en la generacion
    float min_Y = 0;
    float max_Y = 4;
    float min_X = -10;
    float max_X = 5;

    float dirX = generarNumeroaleatorio(min_X,max_X);
    float dirY = -4;

    proyectil* p = armaVolcan->crearProyectil(origenX, origenY, dirX, dirY);
    proyectilesEnemigos.push_back(p);
}

void NivelVolcan::eliminarRocaEnIndice(int indice)
{
    if (indice < 0 || indice >= static_cast<int>(proyectilesEnemigos.size()))
        return;

    delete proyectilesEnemigos[indice];

    proyectilesEnemigos.erase(proyectilesEnemigos.begin() + indice);
}
void NivelVolcan::generarCorazonDesdeArriba()
{
    // Posición del jugador
    float xJug = jugador.getX();

    // Rango horizontal alrededor del jugador
    float xMin = xJug - 300.0;
    float xMax = xJug + 300.0;

    // Por si acaso, limitar un poco
    if (xMin < 0.0f)   xMin = 0.0;
    if (xMax > 1500.0f) xMax = 1500.0;

    float xSpawn = generarNumeroaleatorio(xMin, xMax);
    float ySpawn = -50.0;

    // Modelo de caída: solo gravedad hacia abajo
    ModeloMovimiento* modeloCorazon = new ModeloMovimiento(500); // g fuerte, cae rápido

    // vx = 0, vy = 0 -> empieza quieto y la gravedad lo acelera hacia abajo
    proyectil* c = new proyectil(xSpawn, ySpawn, 0.0f, 0.0f, 0, modeloCorazon);

    corazones.push_back(c);
}


void NivelVolcan::actualizar(float dt)
{
    //Acumular tiempo
    tiempoAcumulado += dt;

    //¿Toca generar una nueva bola de fuego?
    if (tiempoAcumulado >= intervaloGeneracion) {
        tiempoAcumulado = 0;
        generarRoca();
    }

    //Actualizar física de todas las rocas
    for (proyectil* p : proyectilesEnemigos) {
        p->actualizar(dt);
    }

    //Acumulamos tiempo y generamos corazón cada cierto intervalo
    tiempoCorazones += dt;
    if (tiempoCorazones >= intervaloCorazones) {
        tiempoCorazones = 0.0;
        generarCorazonDesdeArriba();
    }

    //Actualizar movimiento de corazones
    actualizarCorazones(dt);

    //Eliminar rocas que se salieron de la pantalla
    int i = 0;
    while (i < static_cast<int>(proyectilesEnemigos.size())) {
        if (proyectilesEnemigos[i]->getY() > 710) {
            delete proyectilesEnemigos[i];
            proyectilesEnemigos.erase(proyectilesEnemigos.begin() + i);
        } else {
            ++i;
        }
    }

}
void NivelVolcan::actualizarCorazones(float dt)
{
    int i = 0;
    while (i < static_cast<int>(corazones.size())) {
        proyectil* c = corazones[i];
        if (!c) {
            ++i;
            continue;
        }

        c->actualizar(dt);

        // Si sale del área visible, lo eliminamos
        if (c->debeDestruirse() || c->getY() > 800.0f) {
            delete c;
            corazones.erase(corazones.begin() + i);
        } else {
            ++i;
        }
    }
}
void NivelVolcan::eliminarCorazonEnIndice(int indice)
{
    if (indice < 0 || indice >= static_cast<int>(corazones.size()))
        return;

    delete corazones[indice];
    corazones.erase(corazones.begin() + indice);
}
