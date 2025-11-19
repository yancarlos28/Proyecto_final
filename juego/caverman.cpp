// caverman.cpp
#include "caverman.h"
#include <QDebug>
caverman::caverman()
    : personaje(120), energia(100), puntuacion(0) {}

void caverman::lanzar() {
    // Notifica al nivel para crear proyectil
    qDebug() << "[Caverman] Solicita lanzar proyectil";
}

void caverman::curar(int valor) {
    // ✅ NO accede directo a vida, usa setter público
    int nuevaVida = getVida() + valor;
    setVida(std::min(VIDA_MAXIMA, nuevaVida)); // Usa constante protegida

    // Para energía (atributo propio)
    energia += valor;
    if (energia > 100) energia = 100;
}

void caverman::debugPrint() const {
    qDebug() << "Caverman pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()  // ✅ Usa getter
    << " energia=" << energia << " pts=" << puntuacion;
}
