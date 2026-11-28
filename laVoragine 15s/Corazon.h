#ifndef CORAZON_H
#define CORAZON_H

#include "ObjetoRecolectable.h"

class Corazon : public ObjetoRecolectable {
private:
    int puntosCuracion;
    float velCorriente;    //velocidad en el rio
    float limiteInferiorY;

public:
    Corazon(float x, float y, float w, float h, int curacion = 25,
            float velRio = 0.0f, float altoVentana = 700.0f)
        : ObjetoRecolectable(x, y, w, h),
        puntosCuracion(curacion),
        velCorriente(velRio),
        limiteInferiorY(altoVentana) {}

    void actualizarFisica(float dt) override {
        if (!activa || recolectado) return;
        if (velCorriente > 0.0f) {
            posY += velCorriente * dt;
            if (posY > limiteInferiorY) {
                desactivar();
            }
        }
    }

    bool intentarRecoger(IndianaJones* indy) override {
        if (!activa || recolectado || !indy) return false;
        if (verificarColision(indy)) {
            recolectado = true;
            indy->curar(puntosCuracion);
            desactivar();
            return true;
        }
        return false;
    }
};

#endif // CORAZON_H
