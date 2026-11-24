#ifndef INDIANAPERSONAJE_H
#define INDIANAPERSONAJE_H

#include "Personaje.h"
#include "PuntoAnclaje.h"

class IndianaJones : public Personaje {
private:
    bool estaColgando;
    bool modoEnfurecido;
    bool puedeEnfurecerse;
    bool estaAtacando;
    bool golpeAplicado;
    int direccionMirada;
    float pivoteX;
    float pivoteY;
    float longitudLatigo;
    float anguloPendulo;
    float velAngular;
    float tiempoEnfurecido;
    float tiempoAtaque;
    float cooldownAtaque;
    const float GRAVEDAD = 450.0f; // Escala en píxeles/s^2

public:
    IndianaJones(float x, float y, float w, float h);

    void actualizarFisica(float dt) override;
    void recibirDano(int cantidad) override;
    void curar(int cantidad) override;

    // movimientos especificos de indy
    void activarModoEnfurecido();
    bool intentarEnganchar(const PuntoAnclaje& punto);
    void engancharLatigo(float pX, float pY, float longitud, float anguloInicial);
    void impulsarPendulo(float direccion, float fuerzaImpulso, float dt);
    void soltarLatigo();
    void saltar(float fuerzaSalto);

    // Combate con latigo (Nivel 2)
    void atacarConLatigo();
    void setDireccionMirada(int dir) { if (dir != 0) direccionMirada = (dir > 0) ? 1 : -1; }
    bool registrarImpactoLatigo();

    // Consultas de estado del personaje
    bool esEnfurecido() const { return modoEnfurecido; }
    bool puedeVerTrayectorias() const { return !modoEnfurecido; }
    bool estaUsandoPendulo() const { return estaColgando; }
    bool getEstaAtacando() const { return estaAtacando; }
    int getDireccionMirada() const { return direccionMirada; }
    int getDanoLatigo() const { return modoEnfurecido ? 50 : 30; }
};

#endif // INDIANAPERSONAJE_H
