#ifndef PALANCA_H
#define PALANCA_H

#include "ObjetoRecolectable.h"

class Palanca : public ObjetoRecolectable {
private:
    bool activada;

public:
    Palanca(float x, float y, float w, float h):
        ObjetoRecolectable(x, y, w, h), activada(false) {}

    void actualizarFisica(float dt) override { (void)dt; }

    void volverAIzquierda() {
        activada = false;
        recolectado = false;
    }

    bool intentarRecoger(IndianaJones* indy) override {
        // Si ya activada, no vuelve a cambiar
        if (!activa || activada || !indy) return false;

        if (verificarColision(indy)) {
            activada = true;
            recolectado = true;
            //permanece en pantalla
            return true;
        }
        return false;
    }

    bool estaActivada() const { return activada; }
    bool estaHaciaLaDerecha() const { return activada; }
};

#endif // PALANCA_H