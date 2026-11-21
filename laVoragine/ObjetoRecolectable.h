#ifndef OBJETORECOLECTABLE_H
#define OBJETORECOLECTABLE_H

#include "EntidadJuego.h"
#include "IndianaJones.h"

class ObjetoRecolectable : public EntidadJuego {
protected:
    bool recolectado;

public:
    ObjetoRecolectable(float x, float y, float w, float h)
        : EntidadJuego(x, y, w, h), recolectado(false) {}

    virtual ~ObjetoRecolectable() = default;

    //metodo para recoger
    virtual bool intentarRecoger(IndianaJones* indy) = 0;

    bool estaRecolectado() const { return recolectado; }
};

#endif // OBJETORECOLECTABLE_H
