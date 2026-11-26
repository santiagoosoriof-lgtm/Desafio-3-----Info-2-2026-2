#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "EntidadJuego.h"
#include "Personaje.h"

class Proyectil : public EntidadJuego {
private:
    float x0;
    float y0;
    float v0;
    float anguloRad;
    float gravedad;
    float tiempoVuelo;
    int dano;
    float limiteX;
    float limiteY;
    bool esLatex;

public:
    Proyectil(float x, float y, float w, float h, float velInicial, float anguloGrados,
              float grav = 450.0f, int dmg = 20, float maxW = 1000.0f, float maxH = 700.0f,
              bool latex = false);

    void actualizarFisica(float dt) override;
    bool impactar(Personaje* objetivo);
    bool esGotaLatex() const { return esLatex; }

    // Permite calcular puntos futuros (tFuturo) para dibujar la línea punteada en Estado Concentrado
    void calcularPuntoEnTiempo(float tFuturo, float& outX, float& outY) const;
};

#endif // PROYECTIL_H

