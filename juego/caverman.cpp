// caverman.cpp
#include "caverman.h"
#include <QDebug>

//constructor
caverman::caverman()
    : personaje(120), energia(100), puntuacion(0),
    enSalto(false), velSaltoY(0.0), pisoY(720){}

int caverman::getEnergia() const {
    return energia; }

int caverman::getPuntuacion() const {
    return puntuacion; }

void caverman::setEnergia(int val) {
    energia = std::min(100, val); }

void caverman::setPuntuacion(int val) {
    puntuacion = val; }


//curar
void caverman::curar(int valor) {

    int nuevaVida = getVida() + valor;
    setVida(std::min(VIDA_MAXIMA, nuevaVida));

    energia += valor;
    if (energia > 100) energia = 100;
}

//para saltar
void caverman::iniciarSalto()
{
    if (!enSalto) {
        enSalto = true;
        velSaltoY = -500;
        pisoY = getY();
    }
}
void caverman::actualizarSalto(float dt)
{
    if (enSalto) {
        float g = 800;

        velSaltoY = velSaltoY + g * dt;

        float x = getX();
        float y = getY();

        y = y + velSaltoY * dt;

        // usar el piso que guardamos al iniciar el salto
        if (y >= pisoY) {
            y = pisoY;
            velSaltoY = 0;
            enSalto = false;
        }
        setPos(x, y);
    }

}
bool caverman::estaSaltando() const {
    return enSalto; }

//debbuging
void caverman::debugPrint() const {
    qDebug() << "Caverman pos=(" << getX() << "," << getY()
    << ") vida=" << getVida()  //
    << " energia=" << energia << " pts=" << puntuacion;
}
