// snowman.cpp
#include "snowman.h"
#include <QDebug>
Snowman::Snowman()
    : escalaTamano(1.0f), faseAtaque(0), temperaturaPercibida(0.0f) {
    setPos(800,300);
    setVida(300);
    setAlerta(true);
}

void Snowman::actuar(float dt) {
    // 1. PERCEPCIÓN: Ya recibida via percibirTemperatura()

    // 2. RAZONAMIENTO
    if (temperaturaPercibida < -10.0f) {
        escalaTamano = 1.5f;
        setFase(2); // Método protegido
    } else if (temperaturaPercibida < -5.0f) {
        escalaTamano = 1.2f;
        setFase(1);
    }

    // 3. ACCIÓN
    lanzarHielo();

    // 4. APRENDIZAJE
    aprenderDeTemperatura(temperaturaPercibida);

    actualizar(dt);
}

void Snowman::lanzarHielo() {
    switch(getFase()) { // ✅ Usa getter protegido
    case 0: qDebug() << "[Snowman] Tiro directo"; break;
    case 1: qDebug() << "[Snowman] Tiro disperso"; break;
    case 2: qDebug() << "[Snowman] Tiro oscilatorio"; break;
    }
}

void Snowman::decidirSiguienteAccion() {
    // Analiza historial para mejorar
    if (historialTemperaturas.size() > 10) {
        //float tempPromedio = std::accumulate(...);
        // Ajusta estrategia...
    }
}

void Snowman::debugPrint() const {
    qDebug() << "Snowman pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()  // ✅ Usa getter
    << " escala=" << escalaTamano << " fase=" << getFase();
}

void Snowman::percibirTemperatura(float temp) { temperaturaPercibida = temp; }

void Snowman::aprenderDeTemperatura(float temp) {
    historialTemperaturas.push_back(temp);
}
