#ifndef NIVELMAMUT_H
#define NIVELMAMUT_H

#include "nivel.h"
#include "mamut.h"

class NivelMamut : public Nivel
{
private:

    mamut jefeMamut;
    float anchoMundo;

public:

    NivelMamut();

    void actualizar(float dt) override;

    mamut& getMamut() { return jefeMamut; }
    const mamut& getMamut() const { return jefeMamut; }

    float getAnchoMundo() const { return anchoMundo; }
};

#endif // NIVELMAMUT_H
