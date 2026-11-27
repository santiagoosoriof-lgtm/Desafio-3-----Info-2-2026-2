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
    ATAQUE_A_DISTANCIA,
    ATURDIDO
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
    bool enSuelo;
    float distanciaJugador;
    float objetivoX;
    float objetivoY;

    // temporizador accion
    float cooldownDisparo;
    float tiempoEntreDisparos;
    float cooldownGolpe;
    float cooldownSalto;
    float tiempoAturdido;
    float timerAtravesarPlataforma;

    // memoria de aprendizaje
    static constexpr size_t MAX_ZONAS_MEMORIA = 10;
    std::array<float, MAX_ZONAS_MEMORIA> zonasEvasionX;
    uint8_t totalZonasAprendidas;
    uint8_t indiceEscritura;
    uint8_t indiceZonaObjetivo;
    float timerCambioZona;
    bool enZonaSospecha;

    float multVelPersecucion;
    int danoGolpeCercano;
    int danoDisparo;
    float duracionAturdido;

    // agente autonomo
    void percibir(const IndianaJones* indy, bool indyHaciendoRuido);
    void razonar();
    Proyectil* actuar(float dt, IndianaJones* indy);
    void verificarZonasAprendidas();

public:
    CapatazMayor(float x, float y, float w, float h, float limIzq, float limDer);

    void actualizarFisica(float dt) override;

    void configurarDificultad(bool esDificil);

    // Accion dispara o no
    Proyectil* actualizarIA(float dt, IndianaJones* indy, bool indyHaciendoRuido = false);

    // aprendizaje agente
    void aprenderZonaEvasion(float xEvasion);

    // conserva lo aprendido asi muera indy y reinicie el nivel
    void reiniciarParaNuevoIntento(float xInicial, float yInicial);
    // Metodo para aturdir al capataz
    void recibirDano(int cantidad) override;
    // estados capataz
    void setEnSuelo(bool estado) { enSuelo = estado; }

    EstadoCapataz getEstado() const { return estadoActual; }
    bool estaSospechando() const { return enZonaSospecha; }
    bool estaEnSuelo() const { return enSuelo; }
    int getDireccion() const { return direccionPatrulla; }
    bool puedeAtravesarPlataforma() const { return timerAtravesarPlataforma > 0.0f; }
};

#endif // CAPATAZMAYOR_H
