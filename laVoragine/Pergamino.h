#ifndef PERGAMINO_H
#define PERGAMINO_H

#include "ObjetoRecolectable.h"
#include <cstdint>

class Pergamino : public ObjetoRecolectable {
private:
    uint8_t parteMapa;      // Numero de pieza (ej. 1 a 5)
    float velCorriente;     // velocidad en el rio
    float limiteInferiorY;

public:
    Pergamino(float x, float y, float w, float h, uint8_t numParte,
              float velRio, float altoVentana = 600.0f);

    void actualizarFisica(float dt) override;
    bool intentarRecoger(IndianaJones* indy) override;

    uint8_t getParteMapa() const { return parteMapa; }
};

#endif // PERGAMINO_H
