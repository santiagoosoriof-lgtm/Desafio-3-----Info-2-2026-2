#include "troncomoa.h"
#include <cmath>

TroncoMOA::TroncoMOA(float x, float y, float w, float h, float amp, float freq, float velY, float faseInicial)
    : Obstaculo(x, y, w, h, false),
    x0(x), y0(y), amplitud(amp), frecuencia(freq),
    fase(faseInicial), velVertical(velY), tiempoAcumulado(0.0f) {}

void TroncoMOA::actualizarFisica(float dt) {
    if (!activa) return;

    tiempoAcumulado += dt;
    // x(t) = x0 + A * cos(w*t + phi)
    posX = x0 + amplitud * std::cos(frecuencia * tiempoAcumulado + fase);
    // y(t) = y0 + vy * t
    posY = y0 + velVertical * tiempoAcumulado;
}

void TroncoMOA::aplicarEfecto(Personaje* objetivo) {
    if (objetivo && verificarColision(objetivo)) {
        objetivo->recibirDano(15);
    }
}

void TroncoMOA::reiniciarPosicion(float nuevaY) {
    y0 = nuevaY;
    posY = nuevaY;
    tiempoAcumulado = 0.0f;
}
