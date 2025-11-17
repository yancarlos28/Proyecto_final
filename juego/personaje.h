#ifndef PERSONAJE_H
#define PERSONAJE_H

using namespace std;

class personaje {
public:
    int vida;
    float posX, posY;
    float velX, velY;

    personaje(int vidaInicial = 100);
    virtual ~personaje();

    virtual void mover(float dx, float dy);
    virtual void actualizar(float dt);
    virtual void recibirDanio(int valor);

    // Para depuración
    virtual void debugPrint() const;
};

#endif // PERSONAJE_H
