#include "GestorNivel1.h"
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <algorithm>

GestorNivel1::GestorNivel1(Dificultad dif, uint8_t metaPergaminos)
    : jugador(nullptr),
    estado(EstadoNivel::EN_CURSO),
    dificultad(dif),
    tiempoRestante(120.0f),
    pergaminosRecolectados(0),
    pergaminosObjetivo(metaPergaminos),
    timerSpawnObstaculo(0.0f),
    timerSpawnDisparoOrilla(1.5f),
    timerSpawnItem(2.0f) {

    if (metaPergaminos == 0) {
        throw std::invalid_argument("La meta de pergaminos debe ser mayor a cero.");
    }

    // Reserva estaticas
    obstaculos.reserve(24);
    proyectilesOrilla.reserve(16);
    recolectables.reserve(12);

    configurarParametrosDificultad();

    // Balsa de Indy inicia en el centro
    jugador = new IndianaJones(ANCHO_MUNDO * 0.5f - 32.0f, ALTO_MUNDO - 160.0f, 64.0f, 64.0f);
}

GestorNivel1::~GestorNivel1() {
    liberarMemoriaTotal();
}

void GestorNivel1::configurarParametrosDificultad() {
    // dificultad: modifica constantes físicas, no solo timers
    if (dificultad == Dificultad::NORMAL) {
        velCorrienteRio = 190.0f;
        factorFuerzaRemolino = 18000.0f;   // Atracción hidrodinámica moderada
        factorFrecuenciaTroncos = 2.0f;    // Oscilación sinusoidal suave
        intervaloDisparoOrilla = 2.6f;
    } else {
        velCorrienteRio = 260.0f;
        factorFuerzaRemolino = 31000.0f;   // remolinos mas fuertes
        factorFrecuenciaTroncos = 3.4f;
        intervaloDisparoOrilla = 1.6f;     // disparos mas frecuentes
    }
}

void GestorNivel1::actualizar(float dt) {
    if (estado != EstadoNivel::EN_CURSO) return;

    //cronómetro de 2 minutos
    tiempoRestante -= dt;
    if (tiempoRestante <= 0.0f) {
        tiempoRestante = 0.0f;
        estado = EstadoNivel::DERROTA_TIEMPO;
        return;
    }

    // temporizadores de los obstaculos
    timerSpawnObstaculo -= dt;
    if (timerSpawnObstaculo <= 0.0f) {
        generarObstaculo();
        timerSpawnObstaculo = (dificultad == Dificultad::NORMAL) ? 1.4f : 0.95f;
    }

    timerSpawnDisparoOrilla -= dt;
    if (timerSpawnDisparoOrilla <= 0.0f) {
        generarDisparoDesdeOrilla();
        timerSpawnDisparoOrilla = intervaloDisparoOrilla;
    }

    timerSpawnItem -= dt;
    if (timerSpawnItem <= 0.0f) {
        generarRecolectable();
        timerSpawnItem = 3.2f;
    }

    // temporizador de estado Enfurecido de Indy
    if (jugador->esEnfurecido()) {
        float yPrevio = jugador->getY();
        float xPrevio = jugador->getX();
        jugador->actualizarFisica(dt);
        jugador->setPosicion(xPrevio, yPrevio);
    }

    // Actualizar obstaculos (TroncoMOA y Remolino) y colisiones
    for (Obstaculo* obs : obstaculos) {
        obs->actualizarFisica(dt);

        // Si es remolino, aplica fuerza de arrastre
        if (Remolino* rem = dynamic_cast<Remolino*>(obs)) {
            rem->aplicarAtraccion(jugador, dt);
        } else {
            obs->aplicarEfecto(jugador);
        }

        if (obs->getY() > ALTO_MUNDO + 80.0f) {
            obs->desactivar();
        }
    }

    // Actualizar proyectiles lanzados desde las orillas
    for (Proyectil* proj : proyectilesOrilla) {
        proj->actualizarFisica(dt);
        proj->impactar(jugador);
    }

    // Actualizar recolectables (Pergaminos y Corazones)
    for (ObjetoRecolectable* item : recolectables) {
        item->actualizarFisica(dt);
        if (item->intentarRecoger(jugador)) {
            // Si es pergamino, suma al progreso del mapa
            if (dynamic_cast<Pergamino*>(item) != nullptr) {
                pergaminosRecolectados++;
                if (pergaminosRecolectados >= pergaminosObjetivo) {
                    estado = EstadoNivel::VICTORIA;
                }
            }
        }
    }

    // Verificar salud de Indy
    if (!jugador->estaActiva() || jugador->getSalud() <= 0) {
        estado = EstadoNivel::DERROTA_SALUD;
    }

    // Liberar memoria de entidades inactivas
    limpiarInactivos();
}

void GestorNivel1::moverJugador(float dirX, float dirY, float dt) {
    if (estado != EstadoNivel::EN_CURSO || !jugador) return;

    float vel = jugador->esEnfurecido() ? 390.0f : 260.0f;
    float nuevaX = jugador->getX() + dirX * vel * dt;
    float nuevaY = jugador->getY() + dirY * vel * dt;

    // Restringir la balsa dentro del rio
    nuevaX = std::clamp(nuevaX, ORILLA_IZQ, ORILLA_DER - jugador->getAncho());
    nuevaY = std::clamp(nuevaY, 80.0f, ALTO_MUNDO - jugador->getAlto() - 20.0f);

    jugador->setPosicion(nuevaX, nuevaY);
}

void GestorNivel1::generarObstaculo() {
    float anchoNavegable = ORILLA_DER - ORILLA_IZQ - 120.0f;
    float xRand = ORILLA_IZQ + 60.0f + static_cast<float>(std::rand() % static_cast<int>(anchoNavegable));

    // 65% probabilidad de TroncoMOA, 35% de Remolino
    if (std::rand() % 100 < 65) {
        float amplitud = 70.0f + static_cast<float>(std::rand() % 60);
        float fase = static_cast<float>(std::rand() % 628) / 100.0f;
        obstaculos.push_back(new TroncoMOA(xRand, -60.0f, 110.0f, 36.0f,
                                           amplitud, factorFrecuenciaTroncos, velCorrienteRio, fase));
    } else {
        obstaculos.push_back(new Remolino(xRand, -90.0f, 95.0f, 95.0f,
                                          factorFuerzaRemolino, 190.0f, velCorrienteRio * 0.85f, ALTO_MUNDO));
    }
}

void GestorNivel1::generarDisparoDesdeOrilla() {
    // Dispara alternando desde la orilla izquierda o derecha hacia la posición de Indy
    bool desdeIzquierda = (std::rand() % 2 == 0);
    float origenX = desdeIzquierda ? (ORILLA_IZQ - 20.0f) : (ORILLA_DER + 20.0f);
    float origenY = 120.0f + static_cast<float>(std::rand() % 450);

    float dx = (jugador->getX() + jugador->getAncho() * 0.5f) - origenX;
    float dy = (jugador->getY() + jugador->getAlto() * 0.5f) - origenY;
    const float g = 450.0f;
    float tVuelo = 1.25f;

    float vx = dx / tVuelo;
    float vy = (-dy + 0.5f * g * tVuelo * tVuelo) / tVuelo;
    float v0 = std::sqrt(vx * vx + vy * vy);
    const float PI = 3.14159265f;
    float anguloGrados = std::atan2(vy, vx) * (180.0f / PI);

    proyectilesOrilla.push_back(new Proyectil(origenX, origenY, 16.0f, 16.0f,
                                              v0, anguloGrados, g, 15, ANCHO_MUNDO, ALTO_MUNDO));
}

void GestorNivel1::generarRecolectable() {
    float anchoNavegable = ORILLA_DER - ORILLA_IZQ - 80.0f;
    float xRand = ORILLA_IZQ + 40.0f + static_cast<float>(std::rand() % static_cast<int>(anchoNavegable));

    // Si Indy tiene menos de 60 de vida, 35% de probabilidad de que aparezca un Corazón
    if (jugador->getSalud() < 60 && (std::rand() % 100 < 35)) {
        recolectables.push_back(new Corazon(xRand, -50.0f, 38.0f, 38.0f, 25, velCorrienteRio, ALTO_MUNDO));
    } else {
        uint8_t numPieza = static_cast<uint8_t>(pergaminosRecolectados + 1);
        recolectables.push_back(new Pergamino(xRand, -50.0f, 42.0f, 42.0f, numPieza, velCorrienteRio, ALTO_MUNDO));
    }
}

// elimina entidades inactivas en sin desplazar memoria
void GestorNivel1::limpiarInactivos() {
    for (size_t i = 0; i < obstaculos.size(); ) {
        if (!obstaculos[i]->estaActiva()) {
            delete obstaculos[i];
            obstaculos[i] = obstaculos.back();
            obstaculos.pop_back();
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < proyectilesOrilla.size(); ) {
        if (!proyectilesOrilla[i]->estaActiva()) {
            delete proyectilesOrilla[i];
            proyectilesOrilla[i] = proyectilesOrilla.back();
            proyectilesOrilla.pop_back();
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < recolectables.size(); ) {
        if (!recolectables[i]->estaActiva()) {
            delete recolectables[i];
            recolectables[i] = recolectables.back();
            recolectables.pop_back();
        } else {
            ++i;
        }
    }
}

void GestorNivel1::liberarMemoriaTotal() {
    delete jugador;
    jugador = nullptr;

    for (Obstaculo* obs : obstaculos) delete obs;
    obstaculos.clear();

    for (Proyectil* proj : proyectilesOrilla) delete proj;
    proyectilesOrilla.clear();

    for (ObjetoRecolectable* item : recolectables) delete item;
    recolectables.clear();
}

void GestorNivel1::reiniciar(Dificultad nuevaDif) {
    liberarMemoriaTotal();
    dificultad = nuevaDif;
    estado = EstadoNivel::EN_CURSO;
    tiempoRestante = 120.0f;
    pergaminosRecolectados = 0;
    timerSpawnObstaculo = 0.0f;
    timerSpawnDisparoOrilla = 1.5f;
    timerSpawnItem = 2.0f;

    configurarParametrosDificultad();
    jugador = new IndianaJones(ANCHO_MUNDO * 0.5f - 32.0f, ALTO_MUNDO - 160.0f, 64.0f, 64.0f);
}
