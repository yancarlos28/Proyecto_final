#ifndef PERSONAJE_H
#define PERSONAJE_H

class personaje {
private:
    int vida;          // ✅ PRIVATE: solo accesible desde aquí
    float posX, posY;  // ✅ PRIVATE
    float velX, velY;  // ✅ PRIVATE

protected:
    static constexpr int VIDA_MAXIMA = 100; // Constante para clases derivadas

public:
    explicit personaje(int vidaInicial = 100);
    virtual ~personaje();

    // GETTERS (lectura pública)
    int getVida() const { return vida; }
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getVelX() const { return velX; }
    float getVelY() const { return velY; }

    // SETTERS (modificación controlada)
    void setVida(int v) {
        vida = (v < 0) ? 0 : v; }
    void setPos(float x, float y) {
        posX = x;
        posY = y; }
    void setVel(float vx, float vy) {
        velX = vx;
        velY = vy; }

    // Métodos de lógica del juego
    virtual void mover(float dx, float dy);
    virtual void actualizar(float dt);
    void recibirDanio(int valor) { setVida(getVida() - valor); } // Usa setter

    // Para depuración
    virtual void debugPrint() const;
};

#endif
