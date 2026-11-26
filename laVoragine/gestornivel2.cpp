#include "GestorNivel2.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

GestorNivel2::GestorNivel2(Dificultad dif)
    : jugador(nullptr),
    capataz(nullptr),
    estado(EstadoNivel::EN_CURSO),
    dificultad(dif),
    palancasActivadas(0),
    indyEnSuelo(false),
    indyCorriendo(false),
    jugadorFueVistoAntes(false),
    timerCalderoIzq(1.0f),
    timerCalderoDer(2.2f) {

    proyectiles.reserve(20);
    items.reserve(6);

    inicializarEscenario();
    configurarDificultad();

    // Indy inicia en la pasarela izquierda (x=80, y=580 -> cae sobre plataforma y=660)
    jugador = new IndianaJones(80.0f, 580.0f, 52.0f, 76.0f);

    // El Capataz Mayor patrulla toda la pasarela derecha (1105 a 1520)
    capataz = new CapatazMayor(1280.0f, 580.0f, 56.0f, 80.0f, 1065.0f, 1520.0f);
}

GestorNivel2::~GestorNivel2() {
    liberarMemoriaTotal();
}

void GestorNivel2::inicializarEscenario() {
    //coordenadas pasarelas
    plataformas = {{
        {   0.0f, 660.0f, 345.0f, 32.0f },
        { 515.0f, 660.0f, 260.0f, 32.0f },
        { 515.0f, 810.0f, 260.0f, 32.0f },
        { 1100.0f, 660.0f, 436.0f, 32.0f }
    }};

    // coordenadas ganchos
    puntosAnclaje = {{
        {  162.0f, 250.0f, 450.0f }, // Argolla izquierda
        {  366.0f, 250.0f, 480.0f }, // Argolla centro-izquierda (cruza primer abismo)
        { 1068.0f, 270.0f, 580.0f }, // Barra colgante derecha (cruza segundo abismo)
        { 1360.0f, 270.0f, 450.0f }  // Trapecio derecho (evadir al Capataz)
    }};

    // coordenadas 3 palancas
    items.push_back(new Palanca(35.0f, 612.0f, 40.0f, 48.0f));   // izq
    items.push_back(new Palanca(625.0f, 762.0f, 40.0f, 48.0f));  // centro abajo
    items.push_back(new Palanca(1465.0f, 612.0f, 40.0f, 48.0f)); // derecha

    // Un corazon de curacion
    items.push_back(new Corazon(630.0f, 615.0f, 36.0f, 36.0f, 35, 0.0f, ALTO_MUNDO));
}

void GestorNivel2::configurarDificultad() {
    if (dificultad == Dificultad::NORMAL) {
        intervaloCalderos = 2.8f;
        velInicialLatex = 560.0f;
    } else {
        // En Difícil, los calderos hierven más rápido y lanzan gotas más altas
        intervaloCalderos = 1.7f;
        velInicialLatex = 660.0f;
    }
}

void GestorNivel2::actualizar(float dt) {
    if (estado != EstadoNivel::EN_CURSO) return;

    // Fisica de Indy
    float yAnterior = jugador->getY();
    jugador->actualizarFisica(dt);

    // Limitar bordes laterales de la pantalla
    if (jugador->getX() < 0.0f) jugador->setPosicion(0.0f, jugador->getY());
    if (jugador->getX() + jugador->getAncho() > ANCHO_MUNDO) {
        jugador->setPosicion(ANCHO_MUNDO - jugador->getAncho(), jugador->getY());
    }

    // Colision con plataformas
    indyEnSuelo = false;
    if (!jugador->estaUsandoPendulo()) {
        resolverColisionesPlataformas(yAnterior);
    }

    // Verificar caida al abismo / calderos
    if (jugador->getY() > Y_ABISMO_MORTAL) {
        capataz->aprenderZonaEvasion(jugador->getX());
        estado = EstadoNivel::DERROTA_SALUD;
        return;
    }

    // Calderos de latex hirviendo
    timerCalderoIzq -= dt;
    if (timerCalderoIzq <= 0.0f) {
        lanzarGotaLatex(176.0f, 835.0f); // Caldero izquierdo
        timerCalderoIzq = intervaloCalderos;
    }

    timerCalderoDer -= dt;
    if (timerCalderoDer <= 0.0f) {
        lanzarGotaLatex(1328.0f, 835.0f); // Caldero derecho
        timerCalderoDer = intervaloCalderos;
    }

    // Agente Autonomo (Capataz Mayor: Percepción -> Razonamiento -> Acción)
    bool indyHaciendoRuido = (indyCorriendo && indyEnSuelo);
    Proyectil* balaCapataz = capataz->actualizarIA(dt, jugador, indyHaciendoRuido);
    if (balaCapataz != nullptr) {
        proyectiles.push_back(balaCapataz);
    }

    // Aprendizaje: Si el Capataz estaba persiguiendo/atacando y pierde de vista a Indy, memoriza la zona
    bool capatazEnAlerta = (capataz->getEstado() != EstadoCapataz::PATRULLA);
    if (jugadorFueVistoAntes && !capatazEnAlerta) {
        capataz->aprenderZonaEvasion(jugador->getX());
    }
    jugadorFueVistoAntes = capatazEnAlerta;

    // Evaluar impacto del latigazo de Indy sobre el Capataz
    if (jugador->getEstaAtacando() && capataz && capataz->estaActiva()) {
        float alcanceLatigo = 90.0f;
        float hitboxX = (jugador->getDireccionMirada() == 1)
                            ? (jugador->getX() + jugador->getAncho())
                            : (jugador->getX() - alcanceLatigo);
        float hitboxY = jugador->getY() + 15.0f;
        float hitboxAlto = jugador->getAlto() - 30.0f;

        // Colision entre el alcance del latigo y el Capataz
        bool colisionaLatigo = (hitboxX < capataz->getX() + capataz->getAncho() &&
                                hitboxX + alcanceLatigo > capataz->getX() &&
                                hitboxY < capataz->getY() + capataz->getAlto() &&
                                hitboxY + hitboxAlto > capataz->getY());

        if (colisionaLatigo && jugador->registrarImpactoLatigo()) {
            capataz->recibirDano(jugador->getDanoLatigo());
            // El Capataz memoriza esta zona como peligrosa para futuros intentos
            capataz->aprenderZonaEvasion(jugador->getX());
        }
    }


    // Actualizar proyectiles (balas y gotas de latex)
    for (Proyectil* proj : proyectiles) {
        proj->actualizarFisica(dt);
        proj->impactar(jugador);
    }

    // Interaccion con Palancas y Corazon
    for (ObjetoRecolectable* item : items) {
        item->actualizarFisica(dt);
        if (item->intentarRecoger(jugador)) {
            if (dynamic_cast<Palanca*>(item) != nullptr) {
                palancasActivadas++;
                // El Capataz aprende la ubicación donde Indy opero el mecanismo
                capataz->aprenderZonaEvasion(jugador->getX());
            }
        }
    }

    // Victoria: 3 palancas activadas + llegar a la zona derecha
    if (palancasActivadas >= TOTAL_PALANCAS && jugador->getX() >= 1420.0f) {
        estado = EstadoNivel::VICTORIA;
        return;
    }

    // Verificar salud de Indy
    if (!jugador->estaActiva() || jugador->getSalud() <= 0) {
        capataz->aprenderZonaEvasion(jugador->getX());
        estado = EstadoNivel::DERROTA_SALUD;
        return;
    }

    limpiarInactivos();
}

void GestorNivel2::resolverColisionesPlataformas(float yAnterior) {
    // Solo aterriza si viene cayendo (velY >= 0)
    if (jugador->getVelY() < 0.0f) return;

    float piesAnteriores = yAnterior + jugador->getAlto();
    float piesActuales = jugador->getY() + jugador->getAlto();
    float centroX = jugador->getX() + jugador->getAncho() * 0.5f;

    for (const Plataforma& plat : plataformas) {
        bool dentroEnX = (centroX >= plat.x && centroX <= plat.x + plat.ancho);
        bool cruzoSuperficie = (piesAnteriores <= plat.y + 10.0f && piesActuales >= plat.y);

        if (dentroEnX && cruzoSuperficie) {
            jugador->setPosicion(jugador->getX(), plat.y - jugador->getAlto());
            jugador->setVelocidad(jugador->getVelX(), 0.0f);
            indyEnSuelo = true;
            break;
        }
    }
}

void GestorNivel2::lanzarGotaLatex(float origenX, float origenY) {
    // angulo casi vertical (entre 75° y 105°) para que suba y caiga en parabola sobre las pasarelas
    float anguloGrados = 75.0f + static_cast<float>(std::rand() % 31);
    proyectiles.push_back(new Proyectil(origenX, origenY, 16.0f, 16.0f,
                                        velInicialLatex, anguloGrados, 450.0f, 15,
                                        ANCHO_MUNDO, ALTO_MUNDO));
}

// --- CONTROLES DE ENTRADA DEL JUGADOR ---

void GestorNivel2::moverJugadorHorizontal(float dirX) {
    if (estado != EstadoNivel::EN_CURSO || !jugador) return;

    if (jugador->estaUsandoPendulo()) {
        indyCorriendo = false;
        return;
    }

    if (dirX > 0.1f) jugador->setDireccionMirada(1);
    else if (dirX < -0.1f) jugador->setDireccionMirada(-1);

    float velBase = 230.0f;
    jugador->setVelocidad(dirX * velBase, jugador->getVelY());
    indyCorriendo = (std::abs(dirX) > 0.1f);
}

void GestorNivel2::atacarJugador() {
    if (estado == EstadoNivel::EN_CURSO && jugador) {
        jugador->atacarConLatigo();
    }
}

void GestorNivel2::saltarJugador() {
    if (estado != EstadoNivel::EN_CURSO || !jugador) return;

    if (jugador->estaUsandoPendulo()) {
        jugador->soltarLatigo();
        indyEnSuelo = false;
    } else if (indyEnSuelo) {
        jugador->saltar(510.0f);
        indyEnSuelo = false;
    }
}

void GestorNivel2::intentarUsarLatigo() {
    if (estado != EstadoNivel::EN_CURSO || !jugador || jugador->estaUsandoPendulo()) return;

    // Busca el punto de anclaje mas cercano dentro de su radio de alcance
    for (const PuntoAnclaje& punto : puntosAnclaje) {
        if (jugador->intentarEnganchar(punto)) {
            indyEnSuelo = false;
            indyCorriendo = false;
            break;
        }
    }
}

void GestorNivel2::impulsarLatigo(float dirX, float dt) {
    if (estado == EstadoNivel::EN_CURSO && jugador && jugador->estaUsandoPendulo()) {
        jugador->impulsarPendulo(dirX, 550.0f, dt);
    }
}

void GestorNivel2::soltarLatigo() {
    if (jugador && jugador->estaUsandoPendulo()) {
        jugador->soltarLatigo();
    }
}

void GestorNivel2::limpiarInactivos() {
    for (size_t i = 0; i < proyectiles.size(); ) {
        if (!proyectiles[i]->estaActiva()) {
            delete proyectiles[i];
            proyectiles[i] = proyectiles.back();
            proyectiles.pop_back();
        } else {
            ++i;
        }
    }

    // Solo se eliminan items recolectables; las Palancas siguen dibujándose
    for (size_t i = 0; i < items.size(); ) {
        if (!items[i]->estaActiva()) {
            delete items[i];
            items[i] = items.back();
            items.pop_back();
        } else {
            ++i;
        }
    }
}

void GestorNivel2::liberarMemoriaTotal() {
    delete jugador;
    jugador = nullptr;

    delete capataz;
    capataz = nullptr;

    for (Proyectil* proj : proyectiles) delete proj;
    proyectiles.clear();

    for (ObjetoRecolectable* item : items) delete item;
    items.clear();
}

void GestorNivel2::reiniciar(Dificultad nuevaDif) {
    dificultad = nuevaDif;
    estado = EstadoNivel::EN_CURSO;
    palancasActivadas = 0;
    indyEnSuelo = false;
    indyCorriendo = false;
    jugadorFueVistoAntes = false;

    configurarDificultad();

    // Limpiar proyectiles e items anteriores
    for (Proyectil* proj : proyectiles) delete proj;
    proyectiles.clear();
    for (ObjetoRecolectable* item : items) delete item;
    items.clear();

    // Restaurar items del nivel
    items.push_back(new Palanca(35.0f, 612.0f, 40.0f, 48.0f));
    items.push_back(new Palanca(625.0f, 762.0f, 40.0f, 48.0f));
    items.push_back(new Palanca(1465.0f, 612.0f, 40.0f, 48.0f));
    items.push_back(new Corazon(630.0f, 615.0f, 36.0f, 36.0f, 35, 0.0f, ALTO_MUNDO));
    // Reiniciar jugador
    delete jugador;
    jugador = new IndianaJones(80.0f, 580.0f, 52.0f, 76.0f);
    // se reinicia el capataz
    // para que conserve en memoria su std::array de zonas aprendidas
    if (capataz) {
        capataz->reiniciarParaNuevoIntento(1280.0f, 580.0f);
    } else {
        capataz = new CapatazMayor(1280.0f, 580.0f, 56.0f, 80.0f, 1065.0f, 1520.0f);
    }
}