#ifndef PERSONAJE_H
#define PERSONAJE_H

class personaje {
private:
    int vida;
    double posX, posY;
    double velX, velY;

protected:
    static constexpr int VIDA_MAXIMA = 100; // Constante para clases derivadas

public:
    explicit personaje(int vidaInicial = 100);
    virtual ~personaje();

    // Getter
    int getVida() const;
    float getX() const;
    float getY() const;
    float getVelX() const;
    float getVelY() const;

    // Setter
    void setVida(int v);
    void setPos(float x, float y);
    void setVel(float vx, float vy);

    // Métodos de lógica del juego
    virtual void actualizar(float dt);
    void recibirDanio(int valor) { setVida(getVida() - valor); }

    // Para depuración
    virtual void debugPrint() const;
};

#endif
