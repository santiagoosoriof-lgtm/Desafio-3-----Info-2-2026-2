#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "EntidadJuego.h"
#include "Personaje.h"

class Obstaculo : public EntidadJuego {
protected:
    bool destructible;

public:
    Obstaculo(float x, float y, float w, float h, bool esDestructible)
        : EntidadJuego(x, y, w, h), destructible(esDestructible) {}

    virtual ~Obstaculo() = default;

    // daño del obstaculo al personaje
    virtual void aplicarEfecto(Personaje* objetivo) = 0;

    bool esDestructible() const { return destructible; }
};

#endif // OBSTACULO_H
