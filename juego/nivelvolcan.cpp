#include "nivelvolcan.h"
#include "modelomovimiento.h"
#include <QDebug>

NivelVolcan::NivelVolcan()
{
    // Posición inicial del caverman
    jugador.setPos(100, 350);  // ajusta según tu escena
    tiempoAcumulado = 0;
    intervaloGeneracion = 1.5f; //cada 1.5 s
    // Modelo de movimiento parabólico (gravedad positiva hacia abajo)
    ModeloMovimiento* modelo = new ModeloMovimiento(9.8);

    // Arma del volcán: daño 10, velocidad 150
    armaVolcan = new Arma(10, 150.0f, modelo);
}
NivelVolcan::~NivelVolcan()
{
    // Borrar todas las rocas
    for (int i = 0; i < rocas.size(); i++) {
        delete rocas[i];
    }
    rocas.clear();

    // Borrar el arma del volcán
    delete armaVolcan;
}

void NivelVolcan::generarRoca()
{
    // Coordenadas de la boca del volcán (ajusta según tu fondo)
    float origenX = 400;  // centro de la escena, por ejemplo
    float origenY = 100;  // arriba

    // Dirección inicial (solo hacia abajo, puedes jugar con esto después)
    float dirX = 0;
    float dirY = 1;

    proyectil* p = armaVolcan->crearProyectil(origenX, origenY, dirX, dirY);
    rocas.push_back(p);

    qDebug() << "[NivelVolcan] Se generó una bola de fuego en" << origenX << origenY;
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
    for (int i = 0; i < rocas.size(); i++) {
        rocas[i]->actualizar(dt);
    }

    // 4) Eliminar rocas que se salieron de la pantalla (por ejemplo y > 600)
    int i = 0;
    while (i < rocas.size()) {
        if (rocas[i]->getY() > 600) { // ajusta al alto de tu escena
            delete rocas[i];
            rocas.erase(rocas.begin() + i);
            // NO incrementamos i, porque ahora hay un nuevo elemento en esa posición
        }
        else {
            i++;
        }
    }

    // Más adelante: aquí irán las colisiones roca ↔ caverman
}
