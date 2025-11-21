#include "proyectil.h"

proyectil::proyectil(float x, float y, float vx, float vy, int dmg, ModeloMovimiento* mod)
    : posX(x), posY(y), velX(vx), velY(vy), danio(dmg), modelo(mod) {}

void proyectil::actualizar(float dt) {
    if (modelo) {
        modelo->aplicar(posX, posY, velX, velY, dt);
    }
}
