#include "IndianaJones.h"
#include <cmath>

IndianaJones::IndianaJones(float x, float y, float w, float h)
    : Personaje(x, y, w, h, 100, 200.0f),
    estaColgando(false), pivoteX(0.0f), pivoteY(0.0f),
    longitudLatigo(100.0f), anguloPendulo(0.0f), velAngular(0.0f),
    modoEnfurecido(false), tiempoEnfurecido(0.0f) {}

void IndianaJones::actualizarFisica(float dt) {
    // temporizador del estado Enfurecido (10s)
    if (modoEnfurecido) {
        tiempoEnfurecido -= dt;
        if (tiempoEnfurecido <= 0.0f) {
            modoEnfurecido = false;
            tiempoEnfurecido = 0.0f;
        }
    }

    // Pendulo simple si esta usando el latigo
    if (estaColgando) {
        // Ecuacion:aceleracion angular alpha = -(g / L) * sin(theta)
        float aceleracionAngular = -(GRAVEDAD / longitudLatigo) * std::sin(anguloPendulo);
        velAngular += aceleracionAngular * dt;
        anguloPendulo += velAngular * dt;

        //coordenadas cartesianas
        posX = pivoteX + longitudLatigo * std::sin(anguloPendulo);
        posY = pivoteY + longitudLatigo * std::cos(anguloPendulo);
    } else {
        // Movimiento normal y gravedad (en modo enfurecido pesa mas al saltar)
        float factorGravedad = modoEnfurecido ? 1.5f : 1.0f;
        velY += (GRAVEDAD * factorGravedad) * dt;

        float multiplicadorVel = modoEnfurecido ? 1.5f : 1.0f; // +50% velocidad
        posX += (velX * multiplicadorVel) * dt;
        posY += velY * dt;
    }
}

void IndianaJones::recibirDano(int cantidad) {
    // En modo enfurecido es inmune a daño
    if (modoEnfurecido) return;
    Personaje::recibirDano(cantidad);
}

void IndianaJones::activarModoEnfurecido() {
    modoEnfurecido = true;
    tiempoEnfurecido = 10.0f;
}

void IndianaJones::engancharLatigo(float pX, float pY, float longitud, float anguloInicial) {
    estaColgando = true;
    pivoteX = pX;
    pivoteY = pY;
    longitudLatigo = longitud;
    anguloPendulo = anguloInicial;
    velAngular = 0.0f;
    velX = 0.0f;
    velY = 0.0f;
}

void IndianaJones::soltarLatigo() {
    if (!estaColgando) return;
    estaColgando = false;
    // Conserva impulso
    float velTangencial = velAngular * longitudLatigo;
    velX = velTangencial * std::cos(anguloPendulo);
    velY = -velTangencial * std::sin(anguloPendulo);
}

void IndianaJones::saltar(float fuerzaSalto) {
    if (!estaColgando) {
        // En modo enfurecido saltar es mas dificil (-25% fuerza)
        float factorSalto = modoEnfurecido ? 0.75f : 1.0f;
        velY = -fuerzaSalto * factorSalto;
    }
}
