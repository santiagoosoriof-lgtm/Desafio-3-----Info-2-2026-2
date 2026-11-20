#include "remolino.h"
#include <cmath>

Remolino::Remolino(float x, float y, float w, float h, float fuerza, float radio, float velRio, float altoVentana)
    : Obstaculo(x, y, w, h, false),
    constanteFuerza(fuerza),
    radioInfluencia(radio),
    velCorriente(velRio),
    limiteInferiorY(altoVentana) {}

void Remolino::actualizarFisica(float dt) {
    if (!activa) return;

    // remolino baja con la corriente del rio
    posY += velCorriente * dt;

    // desaparece cuando llega a la parte inferior
    if (posY > limiteInferiorY) {
        desactivar();
    }
}

void Remolino::aplicarEfecto(Personaje* objetivo) {
    (void)objetivo;
}

void Remolino::aplicarAtraccion(IndianaJones* indy, float dt) {
    if (!indy || !activa || indy->esEnfurecido()) return;

    float centroRemolinoX = posX + ancho * 0.5f;
    float centroRemolinoY = posY + alto * 0.5f;
    float centroIndyX = indy->getX() + indy->getAncho() * 0.5f;
    float centroIndyY = indy->getY() + indy->getAlto() * 0.5f;

    float dx = centroRemolinoX - centroIndyX;
    float dy = centroRemolinoY - centroIndyY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia > 5.0f && distancia <= radioInfluencia) {
        float aceleracion = constanteFuerza / distancia;
        float ax = aceleracion * (dx / distancia);
        float ay = aceleracion * (dy / distancia);

        indy->mover(ax * dt, ay * dt);
    }
}

void Remolino::reiniciarPosicion(float nuevaX, float nuevaY) {
    posX = nuevaX;
    posY = nuevaY;
    activa = true;
}
