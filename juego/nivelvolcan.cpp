#include "nivelvolcan.h"
#include "modelomovimiento.h"
#include <QDebug>

NivelVolcan::NivelVolcan()
{
    // Posición inicial del caverman
    jugador.setPos(50, 720);  // ajusta según tu escena

    tiempoAcumulado = 0;
    intervaloGeneracion = 0.5; //cada 0.5s

    // Modelo de movimiento parabólico (gravedad positiva hacia abajo)
    ModeloMovimiento* modelo = new ModeloMovimiento(9.8);

    // Arma del volcán: daño 10, velocidad 150
    armaVolcan = new Arma(10, 180, modelo);
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
    // Coordenadas de la boca del volcán (ajusta según tu fondo)
    float origenX = 1090;
    float origenY = 210;

    // Pequeña variación en la dirección vertical
    float min = 0;
    float max = 1;

    float r    = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float dirX = -0.5;
    float dirY = min + r * (max - min);

    proyectil* p = armaVolcan->crearProyectil(origenX, origenY, dirX, dirY);
    proyectilesEnemigos.push_back(p);
}

void NivelVolcan::actualizar(float dt)
{
    // 1) Acumular tiempo
    tiempoAcumulado += dt;

    // 2) ¿Toca generar una nueva bola de fuego?
    if (tiempoAcumulado >= intervaloGeneracion) {
        tiempoAcumulado = 0;
        generarRoca();
    }

    // 3) Actualizar física de todas las rocas
    for (proyectil* p : proyectilesEnemigos) {
        p->actualizar(dt);
    }

    // 4) Eliminar rocas que se salieron de la pantalla
    int i = 0;
    while (i < static_cast<int>(proyectilesEnemigos.size())) {
        if (proyectilesEnemigos[i]->getY() > 710) {
            delete proyectilesEnemigos[i];
            proyectilesEnemigos.erase(proyectilesEnemigos.begin() + i);
        } else {
            ++i;
        }
    }

    // Más adelante: aquí irán las colisiones roca ↔ caverman
}
