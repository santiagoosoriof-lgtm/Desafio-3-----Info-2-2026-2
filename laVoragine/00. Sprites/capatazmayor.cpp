#include "CapatazMayor.h"
#include <cmath>
#include <algorithm>

CapatazMayor::CapatazMayor(float x, float y, float w, float h, float limIzq, float limDer)
    : Personaje(x, y, w, h, 150, 110.0f),
    estadoActual(EstadoCapataz::PATRULLA),
    limitePatrullaIzq(limIzq),
    limitePatrullaDer(limDer),
    direccionPatrulla(1),
    radioVisionBase(260.0f),
    radioVisionActual(260.0f),
    radioAuditivo(210.0f),
    rangoAtaqueCercano(65.0f),
    jugadorDetectado(false),
    jugadorColgando(false),
    distanciaJugador(9999.0f),
    objetivoX(0.0f),
    objetivoY(0.0f),
    cooldownDisparo(0.0f),
    tiempoEntreDisparos(1.8f),
    cooldownGolpe(0.0f),
    cooldownSalto(0.0f),
    tiempoAturdido(0.0f),
    timerAtravesarPlataforma(0.0f),
    totalZonasAprendidas(0),
    indiceEscritura(0),
    indiceZonaObjetivo(0),
    timerCambioZona(0.0f),
    enZonaSospecha(false),
    multVelPersecucion(1.85f),
    danoGolpeCercano(20),
    danoDisparo(15),
    duracionAturdido(2.0f){
    zonasEvasionX.fill(0.0f);
}

void CapatazMayor::configurarDificultad(bool esDificil) {
    if (esDificil) {
        radioVisionBase = 360.0f;
        radioAuditivo = 300.0f;
        tiempoEntreDisparos = 1.0f;
        multVelPersecucion = 2.35f;   // Mas rápido que Indy corriendo (258 px/s vs 230 px/s)
        danoGolpeCercano = 35;
        danoDisparo = 25;
        duracionAturdido = 1.3f;
        saludMaxima = 90;             // Requiere 3 golpes de latigo para aturdirse
        salud = saludMaxima;
    } else {
        radioVisionBase = 200.0f;
        radioAuditivo = 210.0f;
        tiempoEntreDisparos = 1.8f;
        multVelPersecucion = 1.85f;
        danoGolpeCercano = 20;
        danoDisparo = 20;
        duracionAturdido = 2.0f;
        saludMaxima = 60;             // Requiere 2 golpes de látigo para aturdirse
        salud = saludMaxima;
    }
}

void CapatazMayor::actualizarFisica(float dt) {
    if (!activa) return;
    velY += 450.0f * dt; // Gravedad
    posX += velX * dt;
    posY += velY * dt;
}

Proyectil* CapatazMayor::actualizarIA(float dt, IndianaJones* indy, bool indyHaciendoRuido) {
    if (!activa || !indy || !indy->estaActiva()) return nullptr;

    if (cooldownDisparo > 0.0f) cooldownDisparo -= dt;
    if (cooldownGolpe > 0.0f) cooldownGolpe -= dt;
    if (cooldownSalto > 0.0f) cooldownSalto -= dt;
    if (timerAtravesarPlataforma > 0.0f) timerAtravesarPlataforma -= dt;

    if (estadoActual == EstadoCapataz::ATURDIDO) {
        tiempoAturdido -= dt;
        if (tiempoAturdido <= 0.0f) {
            tiempoAturdido = 0.0f;
            salud = saludMaxima; // Se recupera con salud completa y lo aprendido en memoria
            estadoActual = EstadoCapataz::PATRULLA;
        }
        return nullptr;
    }

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

    // Solo ve si Indy está frente a el
    bool enConoFrontal = (dx * direccionPatrulla >= -20.0f);
    bool enMismaAltura = (std::abs(dy) < 160.0f);

    bool visto = (enConoFrontal && enMismaAltura && distanciaJugador <= radioVisionActual);
    bool escuchado = (indyHaciendoRuido && distanciaJugador <= radioAuditivo && std::abs(dy) < 180.0f);

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
    case EstadoCapataz::ATURDIDO: {
        velX = 0.0f;
        return nullptr;
    }

    case EstadoCapataz::PATRULLA: {
        float limIzqActivo = limitePatrullaIzq;
        float limDerActivo = limitePatrullaDer;

        // Si ya aprendió zonas donde Indy pasa tiempo, centra su patrulla en ellas
        if (totalZonasAprendidas > 0) {
            timerCambioZona += dt;
            if (timerCambioZona >= 8.0f) {
                timerCambioZona = 0.0f;
                indiceZonaObjetivo = (indiceZonaObjetivo + 1) % totalZonasAprendidas;
            }

            float centroZona = zonasEvasionX[indiceZonaObjetivo];
            limIzqActivo = std::max(20.0f, centroZona - 130.0f);
            limDerActivo = std::min(1510.0f, centroZona + 130.0f);
        }

        // En zona sospechosa camina más lento (75%)
        float factorVel = enZonaSospecha ? 0.75f : 0.9f;
        velX = direccionPatrulla * velocidadBase * factorVel;

        // Gira al llegar a los límites de la zona
        if (posX <= limIzqActivo) {
            direccionPatrulla = 1;
        } else if (posX + ancho >= limDerActivo) {
            direccionPatrulla = -1;
        }

        // Evita salir de los bordes exteriores del mapa
        if (posX < 20.0f) { posX = 20.0f; direccionPatrulla = 1; }
        if (posX + ancho > 1516.0f) { posX = 1516.0f - ancho; direccionPatrulla = -1; }

        float piesCapatazY = posY + alto;
        bool enPlataformaSuperior = (piesCapatazY < 600.0f);
        bool enNivelMedio = (piesCapatazY >= 640.0f && piesCapatazY <= 680.0f);
        bool enPlataformaInferior = (piesCapatazY > 790.0f);
        bool sobrePlataformasCentro = (centroCapatazX >= 515.0f && centroCapatazX <= 775.0f);

        //  Si está arriba y pasa por el centro, baja a las plataformas principales
        if (enSuelo && enPlataformaSuperior && sobrePlataformasCentro) {
            timerAtravesarPlataforma = 0.45f;
            enSuelo = false;
            posY += 20.0f;
            velY = 180.0f;
        }
        //  Si está en la plataforma más baja y necesita salir del centro, salta para subir
        else if (enSuelo && cooldownSalto <= 0.0f && enPlataformaInferior &&
                 ((posX < 535.0f && direccionPatrulla == -1) || (posX + ancho > 755.0f && direccionPatrulla == 1))) {
            velY = -495.0f;
            enSuelo = false;
            cooldownSalto = 0.85f;
        }
        //  Si está en el nivel medio, salta los abismos para cruzar entre pasarelas
        else {
            bool bordeAbismo = enNivelMedio && (
                                   (posX > 265.0f && posX < 305.0f && direccionPatrulla == 1) ||
                                   (posX > 510.0f && posX < 545.0f && direccionPatrulla == -1) ||
                                   (posX > 740.0f && posX < 780.0f && direccionPatrulla == 1) ||
                                   (posX > 1095.0f && posX < 1130.0f && direccionPatrulla == -1));

            if (enSuelo && cooldownSalto <= 0.0f && bordeAbismo) {
                velY = -495.0f;
                enSuelo = false;
                cooldownSalto = 0.85f;
            }
        }
        return nullptr;
    }

    case EstadoCapataz::PERSECUCION: {
        direccionPatrulla = (objetivoX >= centroCapatazX) ? 1 : -1;
        velX = direccionPatrulla * (velocidadBase * multVelPersecucion);

        // Limitar solo a los bordes exteriores de la pantalla (0 a 1536)
        if (posX < 0.0f) { posX = 0.0f; velX = 0.0f; }
        if (posX + ancho > 1536.0f) { posX = 1536.0f - ancho; velX = 0.0f; }

        float piesCapatazY = posY + alto;
        bool enNivelMedio = (piesCapatazY >= 640.0f && piesCapatazY <= 680.0f);

        bool indyMasArriba = (objetivoY < posY - 40.0f);
        bool indyMasAbajo  = (objetivoY > piesCapatazY + 30.0f);
        bool sobrePlataformasCentro = (centroCapatazX >= 515.0f && centroCapatazX <= 775.0f);

        bool bordeAbismo = enNivelMedio && (
                               (posX > 265.0f && posX < 305.0f && direccionPatrulla == 1) ||
                               (posX > 510.0f && posX < 545.0f && direccionPatrulla == -1) ||
                               (posX > 740.0f && posX < 780.0f && direccionPatrulla == 1) ||
                               (posX > 1095.0f && posX < 1130.0f && direccionPatrulla == -1));

        if (enSuelo && indyMasAbajo && sobrePlataformasCentro && piesCapatazY < 790.0f) {
            // Baja atravesando la plataforma hacia la intermedia o inferior
            timerAtravesarPlataforma = 0.45f;
            enSuelo = false;
            posY += 20.0f;
            velY = 180.0f;
        } else if (enSuelo && cooldownSalto <= 0.0f && (indyMasArriba || bordeAbismo)) {
            // Salta si Indy está más arriba o si necesita cruzar un abismo
            velY = -495.0f;
            enSuelo = false;
            cooldownSalto = 0.85f;
        }
        return nullptr;
    }

    case EstadoCapataz::ATAQUE_CERCANO: {
        velX = 0.0f;
        direccionPatrulla = (objetivoX >= centroCapatazX) ? 1 : -1;
        if (cooldownGolpe <= 0.0f) {
            indy->recibirDano(danoGolpeCercano);
            cooldownGolpe = 0.85f;
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

            return new Proyectil(origenX, origenY, 18.0f, 18.0f, v0, anguloGrados, g, danoDisparo, 1536.0f, 1024.0f);
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

void CapatazMayor::recibirDano(int cantidad) {
    if (estadoActual == EstadoCapataz::ATURDIDO) return;

    salud -= cantidad;
    if (salud <= 0) {
        salud = 0;
        velX = 0.0f;
        estadoActual = EstadoCapataz::ATURDIDO;
        tiempoAturdido = duracionAturdido;
    }
}
