#include "Pergamino.h"

Pergamino::Pergamino(float x, float y, float w, float h, uint8_t numParte,
                     float velRio, float altoVentana)
    : ObjetoRecolectable(x, y, w, h),
    parteMapa(numParte),
    velCorriente(velRio),
    limiteInferiorY(altoVentana) {}

void Pergamino::actualizarFisica(float dt) {
    if (!activa || recolectado) return;

    posY += velCorriente * dt;

    // si no se coge, desaparece de la pantalla
    if (posY > limiteInferiorY) {
        desactivar();
    }
}

bool Pergamino::intentarRecoger(IndianaJones* indy) {
    if (!activa || recolectado || !indy) return false;

    if (verificarColision(indy)) {
        recolectado = true;
        desactivar();
        return true;
    }
    return false;
}
