#include "proyectil.h"
#include <cmath>

Proyectil::Proyectil(float x, float y, float w, float h, float velInicial, float anguloGrados,
                     float grav, int dmg, float maxW, float maxH, bool latex)
    : EntidadJuego(x, y, w, h),
    x0(x), y0(y), v0(velInicial), gravedad(grav),
    tiempoVuelo(0.0f), dano(dmg), limiteX(maxW), limiteY(maxH),
    esLatex(latex) {
    const float PI = 3.14159265f;
    anguloRad = anguloGrados * (PI / 180.0f);
}

void Proyectil::actualizarFisica(float dt) {
    if (!activa) return;

    tiempoVuelo += dt;
    calcularPuntoEnTiempo(tiempoVuelo, posX, posY);

    // Si sale de los límites de la ventana, se desactiva para liberar recursos
    if (posX < -50.0f || posX > limiteX + 50.0f || posY > limiteY + 50.0f) {
        desactivar();
    }
}

void Proyectil::calcularPuntoEnTiempo(float tFuturo, float& outX, float& outY) const {
    outX = x0 + (v0 * std::cos(anguloRad)) * tFuturo;
    outY = y0 - (v0 * std::sin(anguloRad)) * tFuturo + 0.5f * gravedad * tFuturo * tFuturo;
}

bool Proyectil::impactar(Personaje* objetivo) {
    if (activa && objetivo && verificarColision(objetivo)) {
        objetivo->recibirDano(dano);
        desactivar();
        return true;
    }
    return false;
}
