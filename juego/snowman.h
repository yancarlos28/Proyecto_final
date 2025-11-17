#ifndef SNOWMAN_H
#define SNOWMAN_H
#include "enemigo.h"

class snowman : public enemigo {
public:
    float escalaTamano;
    int faseAtaque;

    snowman();
    ~snowman() override = default;

    void lanzarHielo();
    void actuar(float dt) override;

    void debugPrint() const override;
};

#endif // SNOWMAN_H
