#include "CapatazMayor.h"
#include <cmath>
#include <algorithm>

CapatazMayor::CapatazMayor(float x, float y, float w, float h, float limIzq, float limDer)
    : Personaje(x, y, w, h, 150, 110.0f),
    estadoActual(EstadoCapataz::PATRULLA),
    limitePatrullaIzq(limIzq),
    limitePatrullaDer(limDer),
    direccionPatrulla(1),
    radioVisionBase(300.0f),
    radioVisionActual(300.0f),
    radioAuditivo(450.0f),
    rangoAtaqueCercano(65.0f),
    jugadorDetectado(false),
    jugadorColgando(false),
    distanciaJugador(9999.0f),
    objetivoX(0.0f),
    objetivoY(0.0f),
    cooldownDisparo(0.0f),
    tiempoEntreDisparos(1.8f),
    cooldownGolpe(0.0f),
    totalZonasAprendidas(0),
    indiceEscritura(0),
    enZonaSospecha(false) {
    zonasEvasionX.fill(0.0f);
}

void CapatazMayor::actualizarFisica(float dt) {
    if (!activa) return;
    posX += velX * dt;
    posY += velY * dt;
}

Proyectil* CapatazMayor::actualizarIA(float dt, IndianaJones* indy, bool indyHaciendoRuido) {
    if (!activa || !indy || !indy->estaActiva()) return nullptr;

    if (cooldownDisparo > 0.0f) cooldownDisparo -= dt;
    if (cooldownGolpe > 0.0f) cooldownGolpe -= dt;

    // Ejecucion de las cualidades del agente
    percibir(indy, indyHaciendoRuido);
    razonar();
    Proyectil* nuevoProyectil = actuar(dt, indy);

    actualizarFisica(dt);
    return nuevoProyectil;
}

// PERCEPCION: Distancia + estimulo auditivo + estado de pendulo
void CapatazMayor::percibir(const IndianaJones* indy, bool indyHaciendoRuido) {
    float centroCapatazX = posX + ancho * 0.5f;
    float centroCapatazY = posY + alto * 0.5f;
    objetivoX = indy->getX() + indy->getAncho() * 0.5f;
    objetivoY = indy->getY() + indy->getAlto() * 0.5f;

    float dx = objetivoX - centroCapatazX;
    float dy = objetivoY - centroCapatazY;
    distanciaJugador = std::sqrt(dx * dx + dy * dy);
    jugadorColgando = indy->estaUsandoPendulo();

    // radio de vista aumentado cuando patrulla zonas aprendidas
    verificarZonasAprendidas();
    radioVisionActual = enZonaSospecha ? (radioVisionBase * 1.25f) : radioVisionBase;

    bool visto = (distanciaJugador <= radioVisionActual);
    bool escuchado = (indyHaciendoRuido && distanciaJugador <= radioAuditivo);

    jugadorDetectado = (visto || escuchado);
}

// RAZONAMIENTO
void CapatazMayor::razonar() {
    if (!jugadorDetectado) {
        estadoActual = EstadoCapataz::PATRULLA;
        return;
    }

    // Si Indy esta columpiandose, no es alcanzable a pie -> Ataque parabolico
    if (jugadorColgando) {
        estadoActual = EstadoCapataz::ATAQUE_A_DISTANCIA;
        return;
    }

    // Diferencia vertical para saber si están en la misma plataforma
    float diffY = std::abs(objetivoY - (posY + alto * 0.5f));

    if (distanciaJugador <= rangoAtaqueCercano && diffY < 50.0f) {
        estadoActual = EstadoCapataz::ATAQUE_CERCANO;
    } else if (diffY >= 90.0f) {
        // Si Indy esta en una plataforma muy alta, dispara a distancia
        estadoActual = EstadoCapataz::ATAQUE_A_DISTANCIA;
    } else {
        estadoActual = EstadoCapataz::PERSECUCION;
    }
}

// ACCION: Movimiento, golpe cuerpo a cuerpo o disparo parabólico calculado
Proyectil* CapatazMayor::actuar(float dt, IndianaJones* indy) {
    (void)dt;
    float centroCapatazX = posX + ancho * 0.5f;

    switch (estadoActual) {
    case EstadoCapataz::PATRULLA: {
        // En zona sospechosa camina más lento (70%) vigilando con más detalle
        float factorVel = enZonaSospecha ? 0.7f : 1.0f;
        velX = direccionPatrulla * velocidadBase * factorVel;

        if (posX <= limitePatrullaIzq) {
            posX = limitePatrullaIzq;
            direccionPatrulla = 1;
        } else if (posX + ancho >= limitePatrullaDer) {
            posX = limitePatrullaDer - ancho;
            direccionPatrulla = -1;
        }
        return nullptr;
    }

    case EstadoCapataz::PERSECUCION: {
        direccionPatrulla = (objetivoX >= centroCapatazX) ? 1 : -1;
        // Duplica su rapidez base para interceptar a Indy
        velX = direccionPatrulla * (velocidadBase * 1.8f);

        // Respeta limites de su plataforma para no caer al vacío
        if (posX < limitePatrullaIzq) { posX = limitePatrullaIzq; velX = 0.0f; }
        if (posX + ancho > limitePatrullaDer) { posX = limitePatrullaDer - ancho; velX = 0.0f; }
        return nullptr;
    }

    case EstadoCapataz::ATAQUE_CERCANO: {
        velX = 0.0f;
        direccionPatrulla = (objetivoX >= centroCapatazX) ? 1 : -1;
        if (cooldownGolpe <= 0.0f) {
            indy->recibirDano(20);
            cooldownGolpe = 1.0f;
        }
        return nullptr;
    }

    case EstadoCapataz::ATAQUE_A_DISTANCIA: {
        velX = 0.0f; // Se detiene para apuntar
        direccionPatrulla = (objetivoX >= centroCapatazX) ? 1 : -1;

        if (cooldownDisparo <= 0.0f) {
            cooldownDisparo = tiempoEntreDisparos;

            // Calculo para interceptar la posición de Indy
            float origenX = centroCapatazX;
            float origenY = posY + alto * 0.3f;
            float dx = objetivoX - origenX;
            float dy = objetivoY - origenY;
            const float g = 450.0f; // Misma gravedad de Proyectil.h

            // Tiempo de vuelo proporcional a la distancia (entre 0.45s y 1.1s)
            float tVuelo = std::clamp(std::abs(dx) / 420.0f, 0.45f, 1.1f);

            float vx = dx / tVuelo;
            // Despeje de y(t) = y0 - vy*t + 0.5*g*t^2
            float vy = (-dy + 0.5f * g * tVuelo * tVuelo) / tVuelo;

            float v0 = std::sqrt(vx * vx + vy * vy);
            const float PI = 3.14159265f;
            float anguloGrados = std::atan2(vy, vx) * (180.0f / PI);

            return new Proyectil(origenX, origenY, 14.0f, 14.0f, v0, anguloGrados, g, 15);
        }
        return nullptr;
    }
    }
    return nullptr;
}

// APRENDIZAJE: guarda las posiciones donde Indy frecuenta
void CapatazMayor::aprenderZonaEvasion(float xEvasion) {
    // Evita guardar coordenadas duplicadas muy cercanas (< 50px)
    for (uint8_t i = 0; i < totalZonasAprendidas; ++i) {
        if (std::abs(zonasEvasionX[i] - xEvasion) < 50.0f) {
            return;
        }
    }

    zonasEvasionX[indiceEscritura] = xEvasion;
    indiceEscritura = (indiceEscritura + 1) % MAX_ZONAS_MEMORIA;
    if (totalZonasAprendidas < MAX_ZONAS_MEMORIA) {
        totalZonasAprendidas++;
    }
}

void CapatazMayor::verificarZonasAprendidas() {
    enZonaSospecha = false;
    float centroX = posX + ancho * 0.5f;

    for (uint8_t i = 0; i < totalZonasAprendidas; ++i) {
        if (std::abs(centroX - zonasEvasionX[i]) <= 70.0f) {
            enZonaSospecha = true;
            break;
        }
    }
}

void CapatazMayor::reiniciarParaNuevoIntento(float xInicial, float yInicial) {
    posX = xInicial;
    posY = yInicial;
    velX = 0.0f;
    velY = 0.0f;
    salud = 150;
    activa = true;
    estadoActual = EstadoCapataz::PATRULLA;
    cooldownDisparo = 0.0f;
    cooldownGolpe = 0.0f;
}
