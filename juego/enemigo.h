#ifndef ENEMIGO_H
#define ENEMIGO_H
#include "personaje.h"

class enemigo : public personaje {
public:
    bool enAlerta;
    int fase;

    enemigo();
    ~enemigo() override = default;

    // comportamiento principal del enemigo (puede sobreescribirse)
    virtual void actuar(float dt);

    void debugPrint() const override;
};

#endif // ENEMIGO_H
