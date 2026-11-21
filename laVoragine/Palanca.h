#ifndef PALANCA_H
#define PALANCA_H

#include "ObjetoRecolectable.h"

class Palanca : public ObjetoRecolectable {
private:
    bool activada;

public:
    Palanca(float x, float y, float w, float h);

    void actualizarFisica(float dt) override { (void)dt; }
    bool intentarRecoger(IndianaJones* indy) override;

    bool estaActivada() const { return activada; }
};

#endif // PALANCA_H