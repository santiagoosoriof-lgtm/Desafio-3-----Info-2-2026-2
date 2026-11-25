#ifndef GESTORNIVEL2_H
#define GESTORNIVEL2_H

#include "IndianaJones.h"
#include "CapatazMayor.h"
#include "Proyectil.h"
#include "PuntoAnclaje.h"
#include "ObjetoRecolectable.h"
#include "Palanca.h"
#include "Corazon.h"
#include "GestorNivel1.h" // Para reutilizar Dificultad y EstadoNivel
#include <array>
#include <vector>
#include <cstdint>

struct Plataforma {
    float x;
    float y;
    float ancho;
    float alto;
};

class GestorNivel2 {
public:
    static constexpr float ANCHO_MUNDO = 1536.0f;
    static constexpr float ALTO_MUNDO = 1024.0f;
    static constexpr float Y_ABISMO_MORTAL = 960.0f; // Caída al vacío/fuego

private:
    IndianaJones* jugador;
    CapatazMayor* capataz;

    // Geometría estática en Stack (0 bytes en Heap, acceso O(1))
    std::array<Plataforma, 4> plataformas;
    std::array<PuntoAnclaje, 4> puntosAnclaje;

    // Entidades dinámicas
    std::vector<Proyectil*> proyectiles;       // Balas del capataz + gotas de látex
    std::vector<ObjetoRecolectable*> items;    // 3 Palancas + Corazón de curación

    // Estado del nivel
    EstadoNivel estado;
    Dificultad dificultad;
    uint8_t palancasActivadas;
    static constexpr uint8_t TOTAL_PALANCAS = 3;

    // Control de estado del jugador en plataforma
    bool indyEnSuelo;
    bool indyCorriendo;
    bool jugadorFueVistoAntes;

    // Temporizadores de calderos de látex
    float timerCalderoIzq;
    float timerCalderoDer;
    float intervaloCalderos;
    float velInicialLatex;

    // Métodos internos
    void inicializarEscenario();
    void configurarDificultad();
    void resolverColisionesPlataformas(float yAnterior);
    void lanzarGotaLatex(float origenX, float origenY);
    void limpiarInactivos();
    void liberarMemoriaTotal();

public:
    GestorNivel2(Dificultad dif = Dificultad::NORMAL);
    ~GestorNivel2();

    GestorNivel2(const GestorNivel2&) = delete;
    GestorNivel2& operator=(const GestorNivel2&) = delete;

    void actualizar(float dt);

    // Controles de entrada desde Qt
    void moverJugadorHorizontal(float dirX);
    void saltarJugador();
    void intentarUsarLatigo();
    void impulsarLatigo(float dirX, float dt);
    void soltarLatigo();
    void atacarJugador();

    // Reinicia el nivel conservando el aprendizaje previo del CapatazMayor
    void reiniciar(Dificultad nuevaDif);

    // Getters de solo lectura para renderizado en Qt
    const IndianaJones* getJugador() const { return jugador; }
    const CapatazMayor* getCapataz() const { return capataz; }
    const std::array<Plataforma, 4>& getPlataformas() const { return plataformas; }
    const std::array<PuntoAnclaje, 4>& getPuntosAnclaje() const { return puntosAnclaje; }
    const std::vector<Proyectil*>& getProyectiles() const { return proyectiles; }
    const std::vector<ObjetoRecolectable*>& getItems() const { return items; }
    bool estaPuertaAbierta() const { return palancasActivadas >= TOTAL_PALANCAS; }

    EstadoNivel getEstado() const { return estado; }
    uint8_t getPalancasActivadas() const { return palancasActivadas; }
    uint8_t getTotalPalancas() const { return TOTAL_PALANCAS; }
};

#endif // GESTORNIVEL2_H
