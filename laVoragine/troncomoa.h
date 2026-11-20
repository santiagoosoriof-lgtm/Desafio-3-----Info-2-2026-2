#ifndef TRONCOMOA_H
#define TRONCOMOA_H

#include "Obstaculo.h"

class TroncoMOA : public Obstaculo {
private:
    float x0;
    float y0;
    float amplitud;
    float frecuencia; // omega = 2 * PI / T
    float fase;
    float velVertical;
    float tiempoAcumulado;

public:
    TroncoMOA(float x, float y, float w, float h, float amp, float freq, float velY, float faseInicial = 0.0f);

    void actualizarFisica(float dt) override;
    void aplicarEfecto(Personaje* objetivo) override;
    void reiniciarPosicion(float nuevaY);
};

#endif // TRONCOMOA_H