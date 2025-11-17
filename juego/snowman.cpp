#include "snowman.h"
#include <iostream>
snowman::snowman()
    : escalaTamano(1.0f), faseAtaque(0) {
    vida = 300;
}

void snowman::lanzarHielo() {
    std::cout << "[JefeSnowman] lanza hielo (fase " << faseAtaque << ")\n";
}

void snowman::actuar(float dt) {}
