// enemigo.h
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "personaje.h"

class enemigo : public personaje {
private:
    bool enAlerta;  // ✅ Private
    int fase;       // ✅ Private

protected:
    // Getter protegido para clases derivadas
    bool getAlerta() const { return enAlerta; }
    void setAlerta(bool alerta) { enAlerta = alerta; }
    void setFase(int f) { fase = f; }

public:
    enemigo();
    ~enemigo() override = default;
    int getFase() const { return fase; }

    // Método polimórfico de comportamiento
    virtual void actuar(float dt);
    void debugPrint() const override;
};

#endif
