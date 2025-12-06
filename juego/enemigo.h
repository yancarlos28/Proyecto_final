// enemigo.h
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "personaje.h"

class enemigo : public personaje {
private:

    bool enAlerta;
    int fase;

protected:

    // Getter protegido para clases derivadas
    bool getAlerta() const;
    void setAlerta(bool alerta);
    void setFase(int f);

public:

    enemigo();
    ~enemigo() override = default;
    int getFase() const { return fase; }

    virtual void actuar(float dt);
    void debugPrint() const override;
};

#endif
