#ifndef INDIANAPERSONAJE_H
#define INDIANAPERSONAJE_H

#include "Personaje.h"

class IndianaJones : public Personaje {
private:
    bool estaColgando;
    float pivoteX;
    float pivoteY;
    float longitudLatigo;
    float anguloPendulo;
    float velAngular;

    bool modoEnfurecido;
    float tiempoEnfurecido; // Temporizador de 10 segundos
    const float GRAVEDAD = 450.0f; // Escala en píxeles/s^2

public:
    IndianaJones(float x, float y, float w, float h);

    void actualizarFisica(float dt) override;
    void recibirDano(int cantidad) override;

    // movimientos especificos de indy
    void activarModoEnfurecido();
    void engancharLatigo(float pX, float pY, float longitud, float anguloInicial);
    void soltarLatigo();
    void saltar(float fuerzaSalto);

    // Consultas de estado del personaje
    bool esEnfurecido() const { return modoEnfurecido; }
    bool puedeVerTrayectorias() const { return !modoEnfurecido; }
    bool estaUsandoPendulo() const { return estaColgando; }
};

#endif // INDIANAPERSONAJE_H
