#ifndef GESTORNIVEL1_H
#define GESTORNIVEL1_H

#include "IndianaJones.h"
#include "Obstaculo.h"
#include "TroncoMOA.h"
#include "Remolino.h"
#include "Proyectil.h"
#include "ObjetoRecolectable.h"
#include "Pergamino.h"
#include "Corazon.h"
#include <vector>
#include <cstdint>

enum class Dificultad : uint8_t {
    NORMAL = 0,
    DIFICIL = 1
};

enum class EstadoNivel : uint8_t {
    EN_CURSO,
    VICTORIA,
    DERROTA_TIEMPO,
    DERROTA_SALUD
};

class GestorNivel1 {
public:
    // Dimensiones lógicas exactas de tus fondos
    static constexpr float ANCHO_MUNDO = 1536.0f;
    static constexpr float ALTO_MUNDO = 1024.0f;
    // Márgenes de las orillas de selva en tu sprite de río (ajusta si es necesario)
    static constexpr float ORILLA_IZQ = 425.0f;
    static constexpr float ORILLA_DER = 1110.0f;

private:
    IndianaJones* jugador;
    std::vector<Obstaculo*> obstaculos;
    std::vector<Proyectil*> proyectilesOrilla;
    std::vector<ObjetoRecolectable*> recolectables;

    // Estado del nivel
    EstadoNivel estado;
    Dificultad dificultad;
    float tiempoRestante;          // 60.0 segundos (1 minuto)
    uint8_t pergaminosRecolectados;
    uint8_t pergaminosObjetivo;    // Ej. 10 piezas para armar el mapa

    // Temporizadores de generación (spawning)
    float timerSpawnObstaculo;
    float timerSpawnDisparoOrilla;
    float timerSpawnItem;

    // Parámetros físicos escalados por dificultad
    float velCorrienteRio;
    float factorFuerzaRemolino;
    float factorFrecuenciaTroncos;
    float intervaloDisparoOrilla;

    // Métodos internos de gestión y limpieza
    void configurarParametrosDificultad();
    void generarObstaculo();
    void generarDisparoDesdeOrilla();
    void generarRecolectable();
    void limpiarInactivos();
    void liberarMemoriaTotal();

public:
    GestorNivel1(Dificultad dif = Dificultad::NORMAL, uint8_t metaPergaminos = 10);
    ~GestorNivel1();

    // Evita copias accidentales de punteros dinamicos (Regla de Tres/Cinco en C++)
    GestorNivel1(const GestorNivel1&) = delete;
    GestorNivel1& operator=(const GestorNivel1&) = delete;

    void actualizar(float dt);
    void moverJugador(float dirX, float dirY, float dt);
    void reiniciar(Dificultad nuevaDif);

    // Getters de solo lectura para que la GUI de Qt dibuje los sprites
    const IndianaJones* getJugador() const { return jugador; }
    const std::vector<Obstaculo*>& getObstaculos() const { return obstaculos; }
    const std::vector<Proyectil*>& getProyectiles() const { return proyectilesOrilla; }
    const std::vector<ObjetoRecolectable*>& getRecolectables() const { return recolectables; }

    EstadoNivel getEstado() const { return estado; }
    float getTiempoRestante() const { return tiempoRestante; }
    uint8_t getPergaminosRecolectados() const { return pergaminosRecolectados; }
    uint8_t getPergaminosObjetivo() const { return pergaminosObjetivo; }
};

#endif // GESTORNIVEL1_H
