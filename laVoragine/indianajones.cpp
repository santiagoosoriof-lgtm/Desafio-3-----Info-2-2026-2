#include "IndianaJones.h"
#include <cmath>

IndianaJones::IndianaJones(float x, float y, float w, float h)
    : Personaje(x, y, w, h, 100, 200.0f),
    estaColgando(false),
    modoEnfurecido(false),
    puedeEnfurecerse(true),
    estaAtacando(false),
    golpeAplicado(false),
    direccionMirada(1),
    pivoteX(0.0f),
    pivoteY(0.0f),
    longitudLatigo(LONGITUD_LATIGO_FIJA),
    anguloPendulo(0.0f),
    velAngular(0.0f),
    tiempoEnfurecido(0.0f),
    tiempoAtaque(0.0f),
    cooldownAtaque(0.0f){}

void IndianaJones::actualizarFisica(float dt) {

    if (cooldownAtaque > 0.0f) cooldownAtaque -= dt;
    if (estaAtacando) {
        tiempoAtaque -= dt;
        if (tiempoAtaque <= 0.0f) {
            estaAtacando = false;
            tiempoAtaque = 0.0f;
        }
    }

    // temporizador del estado Enfurecido (10s)
    if (modoEnfurecido) {
        tiempoEnfurecido -= dt;
        if (tiempoEnfurecido <= 0.0f) {
            modoEnfurecido = false;
            tiempoEnfurecido = 0.0f;
        }
    }

    // Pendulo simple si esta usando el latigo
    if (estaColgando) {
        const float LIMITE_ANGULO = 1.52f; // ~87 grados a cada lado (arco total ~175° bajo la argolla)

        float aceleracionAngular = -(GRAVEDAD / longitudLatigo) * std::sin(anguloPendulo);
        velAngular += aceleracionAngular * dt;
        anguloPendulo += velAngular * dt;

        // Tope físico para que nunca suba por encima de la argolla ni salga de pantalla
        if (anguloPendulo > LIMITE_ANGULO) {
            anguloPendulo = LIMITE_ANGULO;
            if (velAngular > 0.0f) velAngular = 0.0f;
        } else if (anguloPendulo < -LIMITE_ANGULO) {
            anguloPendulo = -LIMITE_ANGULO;
            if (velAngular < 0.0f) velAngular = 0.0f;
        }

        // Coordenadas cartesianas respecto al pivote
        posX = pivoteX + longitudLatigo * std::sin(anguloPendulo);
        posY = pivoteY + longitudLatigo * std::cos(anguloPendulo);
    } else {
        // Movimiento horizontal y gravedad normal en plataformas
        float factorGravedad = modoEnfurecido ? 1.25f : 1.0f;
        velY += (GRAVEDAD * factorGravedad) * dt;

        float multiplicadorVel = modoEnfurecido ? 1.5f : 1.0f;
        posX += (velX * multiplicadorVel) * dt;
        posY += velY * dt;
    }
}

void IndianaJones::recibirDano(int cantidad) {
    if (modoEnfurecido) return; // Inmune mientras esta enfurecido

    Personaje::recibirDano(cantidad);

    // Si sigue vivo pero con salud crítica (<= 30%), se enfurece automáticamente
    if (activa && salud > 0 && salud <= 30 && puedeEnfurecerse) {
        activarModoEnfurecido();
        puedeEnfurecerse = false;
    }
}

void IndianaJones::curar(int cantidad) {
    Personaje::curar(cantidad);
    if (salud > 30) {
        puedeEnfurecerse = true; // Vuelve a estar disponible si recupera salud
    }
}

void IndianaJones::activarModoEnfurecido() {
    modoEnfurecido = true;
    tiempoEnfurecido = 10.0f;
}

bool IndianaJones::intentarEnganchar(const PuntoAnclaje& punto) {
    // Si ya esta colgado, no vuelve a enganchar
    if (estaColgando) return false;

    float dx = posX - punto.x;
    float dy = posY - punto.y;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia <= punto.radioAlcance && distancia > 10.0f && dy > 10.0f) {
        float anguloInicial = std::atan2(dx, dy);
        engancharLatigo(punto.x, punto.y, LONGITUD_LATIGO_FIJA, anguloInicial);
        return true;
    }
    return false;
}

void IndianaJones::engancharLatigo(float pX, float pY, float longitud, float anguloInicial) {
    estaColgando = true;
    pivoteX = pX;
    pivoteY = pY;
    longitudLatigo = longitud;
    anguloPendulo = anguloInicial;
    velAngular = 0.0f;
    velX = 0.0f;
    velY = 0.0f;
}

void IndianaJones::impulsarPendulo(float direccion, float fuerzaImpulso, float dt) {
    // Permite al jugador darse impulso hacia izquierda (-1) o derecha (1) mientras cuelga
    if (estaColgando) {
        velAngular += (direccion * fuerzaImpulso / longitudLatigo) * dt;
        if (velAngular > 3.0f) velAngular = 3.0f;  //limita el balanceo
        else if (velAngular < -3.0f) velAngular = -3.0f;
    }
}

void IndianaJones::soltarLatigo() {
    if (!estaColgando) return;
    estaColgando = false;
    // Conserva impulso
    float velTangencial = velAngular * longitudLatigo;
    velX = velTangencial * std::cos(anguloPendulo);
    velY = -velTangencial * std::sin(anguloPendulo);
    longitudLatigo = LONGITUD_LATIGO_FIJA;
}

void IndianaJones::saltar(float fuerzaSalto) {
    if (!estaColgando) {
        // En modo enfurecido saltar es mas dificil (-25% fuerza)
        float factorSalto = modoEnfurecido ? 0.92f : 1.0f;
        velY = -fuerzaSalto * factorSalto;
    }
}

void IndianaJones::atacarConLatigo() {
    // No puede atacar si ya está atacando, en cooldown o colgado del techo
    if (estaColgando || estaAtacando || cooldownAtaque > 0.0f) return;

    estaAtacando = true;
    golpeAplicado = false;
    tiempoAtaque = 0.25f;   // Duracion latigazo
    cooldownAtaque = 0.55f; // Tiempo mínimo entre ataques
}

bool IndianaJones::registrarImpactoLatigo() {
    if (estaAtacando && !golpeAplicado) {
        golpeAplicado = true;
        return true;
    }
    return false;
}