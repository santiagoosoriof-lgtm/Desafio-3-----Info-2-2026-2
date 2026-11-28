#include "remolino.h"
#include <cmath>
#include <algorithm>

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
    if (!objetivo || !activa) return;

    float centroRemolinoX = posX + ancho * 0.5f;
    float centroRemolinoY = posY + alto * 0.5f;
    float centroObjX = objetivo->getX() + objetivo->getAncho() * 0.5f;
    float centroObjY = objetivo->getY() + objetivo->getAlto() * 0.5f;

    float dx = centroRemolinoX - centroObjX;
    float dy = centroRemolinoY - centroObjY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    // Solo hace daño si llega al nucleo interno del remolino (radio de 35 px)
    if (distancia <= 35.0f) {
        objetivo->recibirDano(15);
    }
}

void Remolino::aplicarAtraccion(IndianaJones* indy, float dt) {
    if (!indy || !activa) return;

    float centroRemolinoX = posX + ancho * 0.5f;
    float centroRemolinoY = posY + alto * 0.5f;
    float centroIndyX = indy->getX() + indy->getAncho() * 0.5f;
    float centroIndyY = indy->getY() + indy->getAlto() * 0.5f;

    float dx = centroRemolinoX - centroIndyX;
    float dy = centroRemolinoY - centroIndyY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia > 5.0f && distancia <= radioInfluencia) {
        float distanciaEfectiva = std::max(distancia, 55.0f);
        float velArrastre = std::min(constanteFuerza / distanciaEfectiva, 235.0f);

        float vx = velArrastre * (dx / distancia);
        float vy = velArrastre * (dy / distancia);

        indy->mover(vx * dt, vy * dt);
    }
}

void Remolino::reiniciarPosicion(float nuevaX, float nuevaY) {
    posX = nuevaX;
    posY = nuevaY;
    activa = true;
}
