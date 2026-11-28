#ifndef REMOLINO_H
#define REMOLINO_H

#include "Obstaculo.h"
#include "IndianaJones.h"

class Remolino : public Obstaculo {
private:
    float constanteFuerza;
    float radioInfluencia;
    float velCorriente;
    float limiteInferiorY;

public:
    Remolino(float x, float y, float w, float h, float fuerza, float radio, float velRio, float altoVentana = 600.0f);

    void actualizarFisica(float dt) override;
    void aplicarEfecto(Personaje* objetivo) override;
    void aplicarAtraccion(IndianaJones* indy, float dt);

    // reutiliza el objeto en la ventana
    void reiniciarPosicion(float nuevaX, float nuevaY);
};

#endif // REMOLINO_H
