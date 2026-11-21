#ifndef CAPATAZMAYOR_H
#define CAPATAZMAYOR_H

#include "Personaje.h"
#include "IndianaJones.h"
#include "Proyectil.h"
#include <array>
#include <cstdint>

// 1 byte en memoria (uint8_t)
enum class EstadoCapataz : uint8_t {
    PATRULLA,
    PERSECUCION,
    ATAQUE_CERCANO,
    ATAQUE_A_DISTANCIA
};

class CapatazMayor : public Personaje {
private:
    // estado y patrulla
    EstadoCapataz estadoActual;
    float limitePatrullaIzq;
    float limitePatrullaDer;
    int direccionPatrulla;

    // percepcion
    float radioVisionBase;
    float radioVisionActual;
    float radioAuditivo;
    float rangoAtaqueCercano;

    // Datos percibidos
    bool jugadorDetectado;
    bool jugadorColgando;
    float distanciaJugador;
    float objetivoX;
    float objetivoY;

    // temporizador accion
    float cooldownDisparo;
    float tiempoEntreDisparos;
    float cooldownGolpe;

    // memoria de aprendizaje
    static constexpr size_t MAX_ZONAS_MEMORIA = 5;
    std::array<float, MAX_ZONAS_MEMORIA> zonasEvasionX;
    uint8_t totalZonasAprendidas;
    uint8_t indiceEscritura;
    bool enZonaSospecha;

    // agente autonomo
    void percibir(const IndianaJones* indy, bool indyHaciendoRuido);
    void razonar();
    Proyectil* actuar(float dt, IndianaJones* indy);
    void verificarZonasAprendidas();

public:
    CapatazMayor(float x, float y, float w, float h, float limIzq, float limDer);

    void actualizarFisica(float dt) override;

    // Accion dispara o no
    Proyectil* actualizarIA(float dt, IndianaJones* indy, bool indyHaciendoRuido = false);

    // aprendizaje agente
    void aprenderZonaEvasion(float xEvasion);

    // conserva lo aprendido asi cambie de nivel
    void reiniciarParaNuevoIntento(float xInicial, float yInicial);

    // estados capataz
    EstadoCapataz getEstado() const { return estadoActual; }
    bool estaSospechando() const { return enZonaSospecha; }
    int getDireccion() const { return direccionPatrulla; }
};

#endif // CAPATAZMAYOR_H
