#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    timerJuego(new QTimer(this)),
    escenaN1(new QGraphicsScene(0, 0, 1536, 1024, this)),
    escenaN2(new QGraphicsScene(0, 0, 1536, 1024, this)),
    fondoN1_A(nullptr),
    fondoN1_B(nullptr),
    fondoN2(nullptr),
    scrollRioY(0.0f),
    contadorFrames(0),
    dirBalsaX(0),
    nivel1(nullptr),
    nivel2(nullptr),
    dificultadActual(Dificultad::NORMAL),
    nivelActivo(0),
    saludAnterior(100),
    pergaminosAnteriores(0),
    palancasAnteriores(0),
    proyectilesAnterioresN2(0){

    ui->setupUi(this);

    // Conectar escenas a los QGraphicsView
    ui->viewNivel1->setScene(escenaN1);
    ui->viewNivel2->setScene(escenaN2);

    ui->viewNivel1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->viewNivel1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->viewNivel1->setFocusPolicy(Qt::NoFocus);

    ui->viewNivel2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->viewNivel2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->viewNivel2->setFocusPolicy(Qt::NoFocus);

    // Cargar fondos y sprites desde el .qrc una sola vez
    cargarFondos();
    cargarSprites();

    configurarAudio();

    // Iniciar en la página 0 (Menu Principal)
    ui->stackedWidget->setCurrentIndex(0);

    //Focus de acciones de los botones
    ui->btnNormal->setFocusPolicy(Qt::NoFocus);
    ui->btnDificil->setFocusPolicy(Qt::NoFocus);
    ui->btnReintentar->setFocusPolicy(Qt::NoFocus);
    ui->btnVolverMenu->setFocusPolicy(Qt::NoFocus);

    connect(timerJuego, &QTimer::timeout, this, &MainWindow::actualizarJuego);
}

MainWindow::~MainWindow() {
    timerJuego->stop();
    limpiarItemsFrame(escenaN1, itemsFrameN1);
    limpiarItemsFrame(escenaN2, itemsFrameN2);
    delete nivel1;
    delete nivel2;
    delete ui;
}


// CARGA DE FONDOS Y SPRITES CON LAS RUTAS EXACTAS .QRC

void MainWindow::cargarFondos() {
    scrollRioY = 0.0f;

    // Fondo Nivel 1 (Rio Putumayo)
    QPixmap pixRio(":/0. Fondo Nivel I.png");
    if (pixRio.isNull()) {
        pixRio = QPixmap(1536, 1024);
        pixRio.fill(QColor(34, 95, 115));
    } else {
        pixRio = pixRio.scaled(1536, 1024, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    fondoN1_A = escenaN1->addPixmap(pixRio);
    fondoN1_B = escenaN1->addPixmap(pixRio);
    fondoN1_A->setZValue(-10);
    fondoN1_B->setZValue(-10);
    fondoN1_A->setPos(0, 0);
    fondoN1_B->setPos(0, -1024);

    // Fondo Nivel 2 (La Chorrera)
    QPixmap pixChorrera(":/0. Fondo inivel II.png");
    if (pixChorrera.isNull()) {
        pixChorrera = QPixmap(1536, 1024);
        pixChorrera.fill(QColor(65, 50, 38));
    } else {
        pixChorrera = pixChorrera.scaled(1536, 1024, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    fondoN2 = escenaN2->addPixmap(pixChorrera);
    fondoN2->setZValue(-10);
    fondoN2->setPos(0, 0);
}

void MainWindow::cargarSprites() {
    // --- Nivel 1 ---
    sprites["balsa_frente"]   = QPixmap(":/0. Balsa de frente.png");
    sprites["balsa_der"]      = QPixmap(":/0. Balsa derecha.png");
    sprites["balsa_izq"]      = QPixmap(":/0. Balsa izquierda.png");
    sprites["balsa_impacto"]  = QPixmap(":/0. Balsa impacto.png");

    sprites["tronco_centro"]  = QPixmap(":/00. Tronco.png");
    sprites["tronco_der"]     = QPixmap(":/00.Tronco derecha.png");
    sprites["tronco_izq"]     = QPixmap(":/00.Tronco izq.png");
    sprites["remolino"]       = QPixmap(":/00. Remolino.png");

    // --- items ---
    sprites["corazon"]        = QPixmap(":/0. Corazon.png");
    sprites["salud_capataz"]  = QPixmap(":/0. SaludCapataz.png");
    sprites["palanca_izq"]    = QPixmap(":/0. Palancaizq.png");
    sprites["palanca_der"]    = QPixmap(":/0. Palancader.png");
    sprites["pergamino"]      = QPixmap(":/00. pergamino.png");


    // --- Nivel 2: Indiana Jones ---
    sprites["indy_parado"]    = QPixmap(":/00. Indy parado.png");
    sprites["indy_camina1"]   = QPixmap(":/00. Indy camina1.png");
    sprites["indy_camina2"]   = QPixmap(":/00. Indy camina2.png");
    sprites["pendulo_centro"] = QPixmap(":/00.Pendulo centro.png");
    sprites["pendulo_der"]    = QPixmap(":/00.Penduloder.png");
    sprites["pendulo_izq"]    = QPixmap(":/00.Penduloizq.png");

    // --- Nivel 2: Capataz Mayor ---
    sprites["cap_parado"]     = QPixmap(":/00. Capataz parado.png");
    sprites["cap_camina1"]    = QPixmap(":/00. Capataz camina1.png");
    sprites["cap_camina2"]    = QPixmap(":/00. Capataz camina2.png");
    sprites["cap_camina3"]    = QPixmap(":/00. Capataz camina3.png");
    sprites["cap_prepara"]    = QPixmap(":/00. Capataz prepara proyectil.png");
    sprites["cap_dispara"]    = QPixmap(":/00. Capataz lanzaproyectil.png");
    sprites["cap_atacado"]    = QPixmap(":/00. Capataz atacado.png");
}

QPixmap MainWindow::obtenerSprite(const QString &clave, int ancho, int alto, bool voltearHorizontal) {
    QPixmap pix = sprites.value(clave);
    if (pix.isNull()) return QPixmap();

    pix = pix.scaled(ancho, alto, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (voltearHorizontal) {
        pix = pix.transformed(QTransform().scale(-1, 1));
    }
    return pix;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (ui->viewNivel1) ui->viewNivel1->fitInView(escenaN1->sceneRect(), Qt::KeepAspectRatio);
    if (ui->viewNivel2) ui->viewNivel2->fitInView(escenaN2->sceneRect(), Qt::KeepAspectRatio);
}


// TRANSICIONES ENTRE PANTALLAS (QStackedWidget)

void MainWindow::on_btnNormal_clicked() {
    dificultadActual = Dificultad::NORMAL;
    iniciarNivel1();
}

void MainWindow::on_btnDificil_clicked() {
    dificultadActual = Dificultad::DIFICIL;
    iniciarNivel1();
}

void MainWindow::iniciarNivel1() {
    if (!nivel1) nivel1 = new GestorNivel1(dificultadActual);
    else nivel1->reiniciar(dificultadActual);

    saludAnterior = 100;
    pergaminosAnteriores = 0;
    if (!musicaFondo->isPlaying()) musicaFondo->play();

    nivelActivo = 1;
    dirBalsaX = 0;
    teclasPresionadas.clear();
    ui->stackedWidget->setCurrentIndex(1);
    ui->viewNivel1->fitInView(escenaN1->sceneRect(), Qt::KeepAspectRatio);
    this->setFocus();
    timerJuego->start(16);
}

void MainWindow::iniciarNivel2() {
    if (!nivel2) nivel2 = new GestorNivel2(dificultadActual);
    else nivel2->reiniciar(dificultadActual);

    saludAnterior = 100;
    palancasAnteriores = 0;
    proyectilesAnterioresN2 = 0;
    if (!musicaFondo->isPlaying()) musicaFondo->play();

    nivelActivo = 2;
    teclasPresionadas.clear();
    ui->stackedWidget->setCurrentIndex(2);
    ui->viewNivel2->fitInView(escenaN2->sceneRect(), Qt::KeepAspectRatio);
    this->setFocus();
    timerJuego->start(16);
}

void MainWindow::mostrarPantallaFinal(const QString &mensaje) {
    timerJuego->stop();
    nivelActivo = 0;
    teclasPresionadas.clear();
    ui->lblResultadoFinal->setText(mensaje);
    ui->stackedWidget->setCurrentIndex(3);
    musicaFondo->stop();
}

void MainWindow::on_btnReintentar_clicked() {
    if (nivel2 != nullptr && nivel2->getEstado() != EstadoNivel::EN_CURSO) {
        iniciarNivel2();
    } else {
        iniciarNivel1();
    }
}

void MainWindow::on_btnVolverMenu_clicked() {
    timerJuego->stop();
    nivelActivo = 0;
    teclasPresionadas.clear();
    ui->stackedWidget->setCurrentIndex(0);
    musicaFondo->stop();
}


// BUCLE PRINCIPAL DEL JUEGO Y CONTROL DE TECLADO

void MainWindow::actualizarJuego() {
    const float dt = 0.016f;
    contadorFrames++;

    if (nivelActivo == 1 && nivel1) {
        float dx = 0.0f, dy = 0.0f;
        if (teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left))  dx -= 1.0f;
        if (teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right)) dx += 1.0f;
        if (teclasPresionadas.contains(Qt::Key_W) || teclasPresionadas.contains(Qt::Key_Up))    dy -= 1.0f;
        if (teclasPresionadas.contains(Qt::Key_S) || teclasPresionadas.contains(Qt::Key_Down))  dy += 1.0f;

        dirBalsaX = (dx < -0.1f) ? -1 : (dx > 0.1f ? 1 : 0);

        nivel1->moverJugador(dx, dy, dt);
        nivel1->actualizar(dt);
        dibujarNivel1();

        if (nivel1->getEstado() == EstadoNivel::VICTORIA) {
            iniciarNivel2();
        } else if (nivel1->getEstado() == EstadoNivel::DERROTA_TIEMPO) {
            mostrarPantallaFinal("¡Se acabó el tiempo en el Río Putumayo!");
        } else if (nivel1->getEstado() == EstadoNivel::DERROTA_SALUD) {
            mostrarPantallaFinal("¡La balsa fue destruida en los rápidos!");
        }
    }
    else if (nivelActivo == 2 && nivel2) {
        float dx = 0.0f;
        if (teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left))  dx -= 1.0f;
        if (teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right)) dx += 1.0f;

        if (nivel2->getJugador()->estaUsandoPendulo()) {
            nivel2->impulsarLatigo(dx, dt);
        } else {
            nivel2->moverJugadorHorizontal(dx);
        }

        nivel2->actualizar(dt);
        dibujarNivel2();

        if (nivel2->getEstado() == EstadoNivel::VICTORIA) {
            mostrarPantallaFinal("¡Prisioneros liberados! Has escapado de La Chorrera.");
        } else if (nivel2->getEstado() == EstadoNivel::DERROTA_SALUD) {
            mostrarPantallaFinal("¡Has caido en La Chorrera! Intentalo de nuevo.");
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    teclasPresionadas.insert(event->key());

    if (nivelActivo == 2 && nivel2) {
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
            nivel2->saltarJugador();
        }
        if (event->key() == Qt::Key_E) {
            nivel2->intentarUsarLatigo();
            if (nivel2->getJugador()->estaUsandoPendulo()) {
                fxLatigo->play();
            }
        }
        if (event->key() == Qt::Key_J || event->key() == Qt::Key_F) {
            nivel2->atacarJugador();
            fxLatigo->play();
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    teclasPresionadas.remove(event->key());
}

// FUNCIONES DE RENDERIZADO DE LOS SPRITES

void MainWindow::limpiarItemsFrame(QGraphicsScene *escena, QList<QGraphicsItem*> &lista) {
    for (QGraphicsItem *item : lista) {
        escena->removeItem(item);
        delete item;
    }
    lista.clear();
}
void MainWindow::dibujarTrayectoriaPredictiva(QGraphicsScene *escena, QList<QGraphicsItem*> &lista, const Proyectil *proj) {
    for (int i = 1; i <= 8; ++i) {
        float tFuturo = i * 0.12f;
        float px = 0.0f, py = 0.0f;
        proj->calcularPuntoEnTiempo(tFuturo, px, py);
        auto *punto = escena->addEllipse(px - 3, py - 3, 6, 6,
                                         QPen(Qt::yellow), QBrush(Qt::yellow));
        punto->setZValue(5);
        lista.append(punto);
    }
}
void MainWindow::dibujarNivel1() {
    if (!nivel1) return;
    // 1. Scroll vertical infinito del río
    scrollRioY += 3.2f;
    if (scrollRioY >= 1024.0f) scrollRioY -= 1024.0f;
    fondoN1_A->setPos(0, scrollRioY);
    fondoN1_B->setPos(0, scrollRioY - 1024.0f);

    // 2. Actualizar parte superior de  la pantalla
    const IndianaJones *indy = nivel1->getJugador();
    ui->barraSaludN1->setValue(indy->getSalud());
    int segTotales = static_cast<int>(std::ceil(nivel1->getTiempoRestante()));
    ui->lblTiempoN1->setText(QString("Tiempo: %1:%2")
                                 .arg(segTotales / 60, 2, 10, QChar('0'))
                                 .arg(segTotales % 60, 2, 10, QChar('0')));
    ui->lblPergaminosN1->setText(QString("Pergaminos: %1 / %2")
                                     .arg(nivel1->getPergaminosRecolectados())
                                     .arg(nivel1->getPergaminosObjetivo()));
    limpiarItemsFrame(escenaN1, itemsFrameN1);

    //2.1 efectos de sonido proyectiles, golpes, pergaminos y corazones
    int saludActual = indy->getSalud();
    uint8_t pergaminosActuales = nivel1->getPergaminosRecolectados();

    // Sonido de golpe (impacto de tronco o bala desde la orilla)
    if (saludActual < saludAnterior) {
        fxGolpe->play();
    }
    // Sonido de item (recogió un corazón de curación o un pergamino)
    if (saludActual > saludAnterior || pergaminosActuales > pergaminosAnteriores) {
        fxItem->play();
    }

    saludAnterior = saludActual;
    pergaminosAnteriores = pergaminosActuales;


    // 3. Obstáculos (Troncos MOA con inclinación alterna y Remolinos giratorios)
    for (const Obstaculo *obs : nivel1->getObstaculos()) {
        if (!obs->estaActiva()) continue;
        if (dynamic_cast<const Remolino*>(obs) != nullptr) {
            QPixmap pixRem = obtenerSprite("remolino", obs->getAncho(), obs->getAlto());
            if (!pixRem.isNull()) {
                auto *item = escenaN1->addPixmap(pixRem);
                item->setTransformOriginPoint(obs->getAncho() * 0.5, obs->getAlto() * 0.5);
                item->setRotation((contadorFrames * 6) % 360); // Efecto giro continuo
                item->setPos(obs->getX(), obs->getY());
                itemsFrameN1.append(item);
            }
        } else {
            // Alterna sprite del tronco segun la fase de animación
            int faseTronco = (contadorFrames / 18) % 3;
            QString claveTronco = (faseTronco == 0) ? "tronco_izq"
                                  : (faseTronco == 1) ? "tronco_centro" : "tronco_der";
            QPixmap pixTronco = obtenerSprite(claveTronco, obs->getAncho(), obs->getAlto());
            if (!pixTronco.isNull()) {
                auto *item = escenaN1->addPixmap(pixTronco);
                item->setPos(obs->getX(), obs->getY());
                itemsFrameN1.append(item);
            }
        }
    }

    // 4. Recolectables (Corazones y Pergaminos)
    for (const ObjetoRecolectable *rec : nivel1->getRecolectables()) {
        if (!rec->estaActiva()) continue;

        bool esCorazon = (dynamic_cast<const Corazon*>(rec) != nullptr);
        QPixmap pixRec = obtenerSprite(esCorazon ? "corazon" : "pergamino",
                                       rec->getAncho() + 10, rec->getAlto() + 10);
        if (!pixRec.isNull()) {
            auto *item = escenaN1->addPixmap(pixRec);
            item->setPos(rec->getX() - 5, rec->getY() - 5);
            itemsFrameN1.append(item);
        }
    }

    // 5. Proyectiles desde las orillas + Trayectorias en Estado Concentrado
    for (const Proyectil *proj : nivel1->getProyectiles()) {
        if (!proj->estaActiva()) continue;
        auto *bala = escenaN1->addEllipse(proj->getX(), proj->getY(), proj->getAncho(), proj->getAlto(),
                                          QPen(Qt::black, 2), QBrush(QColor(45, 45, 45)));
        itemsFrameN1.append(bala);
        if (indy->puedeVerTrayectorias()) {
            dibujarTrayectoriaPredictiva(escenaN1, itemsFrameN1, proj);
        }
    }
    // 6. Balsa de Indiana Jones (segun dirección o estado Enfurecido/Impacto)
    QString claveBalsa = "balsa_frente";
    if (indy->esEnfurecido()) {
        claveBalsa = "balsa_impacto";
    } else if (dirBalsaX < 0) {
        claveBalsa = "balsa_izq";
    } else if (dirBalsaX > 0) {
        claveBalsa = "balsa_der";
    }
    QPixmap pixBalsa = obtenerSprite(claveBalsa, indy->getAncho() + 26, indy->getAlto() + 26);
    if (!pixBalsa.isNull()) {
        auto *balsa = escenaN1->addPixmap(pixBalsa);
        balsa->setPos(indy->getX() - 13, indy->getY() - 13);
        itemsFrameN1.append(balsa);
    }
}
void MainWindow::dibujarNivel2() {
    if (!nivel2) return;
    const IndianaJones *indy = nivel2->getJugador();
    const CapatazMayor *capataz = nivel2->getCapataz();
    // 1. Actualizar parte superior
    ui->barraSaludN2->setValue(indy->getSalud());
    ui->barraSaludCapataz->setValue(capataz->estaActiva() ? capataz->getSalud() : 0);
    ui->lblPalancasN2->setText(QString("Palancas: %1 / %2")
                                   .arg(nivel2->getPalancasActivadas())
                                   .arg(nivel2->getTotalPalancas()));
    limpiarItemsFrame(escenaN2, itemsFrameN2);

    // 1.1 Efectos de sonido disparo, golpe, corazon y palanca
    int saludActual = indy->getSalud();
    uint8_t palancasActuales = nivel2->getPalancasActivadas();
    size_t proyectilesActuales = nivel2->getProyectiles().size();

    if (proyectilesActuales > proyectilesAnterioresN2 &&
        capataz && capataz->getEstado() == EstadoCapataz::ATAQUE_A_DISTANCIA) {
        fxDisparo->play();
    }

    if (saludActual < saludAnterior) {
        fxGolpe->play();
    }

    if (saludActual > saludAnterior) {
        fxItem->play();
    }

    if (palancasActuales > palancasAnteriores) {
        fxPalanca->play();
    }

    saludAnterior = saludActual;
    palancasAnteriores = palancasActuales;
    proyectilesAnterioresN2 = proyectilesActuales;

    // 2. Puerta de escape iluminada a la derecha cuando las 3 palancas estan hacia la derecha
    if (nivel2->estaPuertaAbierta()) {
        auto *puerta = escenaN2->addRect(1435, 520, 88, 140,
                                         QPen(Qt::green, 4), QBrush(QColor(50, 255, 100, 110)));
        itemsFrameN2.append(puerta);
    }
    // 3. Palancas (0. Palancaizq.png / 0. Palancader.png) y Corazón (0. Corazon.png)
    for (const ObjetoRecolectable *item : nivel2->getItems()) {
        if (!item->estaActiva()) continue;
        if (const Palanca *pal = dynamic_cast<const Palanca*>(item)) {
            QString clavePal = pal->estaHaciaLaDerecha() ? "palanca_der" : "palanca_izq";
            QPixmap pixPal = obtenerSprite(clavePal, pal->getAncho() + 14, pal->getAlto() + 14);
            if (!pixPal.isNull()) {
                auto *spritePal = escenaN2->addPixmap(pixPal);
                spritePal->setPos(pal->getX() - 7, pal->getY() - 10);
                itemsFrameN2.append(spritePal);
            }
        } else {
            QPixmap pixCor = obtenerSprite("corazon", item->getAncho(), item->getAlto());
            if (!pixCor.isNull()) {
                auto *spriteCor = escenaN2->addPixmap(pixCor);
                spriteCor->setPos(item->getX(), item->getY());
                itemsFrameN2.append(spriteCor);
            }
        }
    }
    // 4. Proyectiles (balas del Capataz + gotas de latex hirviendo)
    for (const Proyectil *proj : nivel2->getProyectiles()) {
        if (!proj->estaActiva()) continue;
        auto *gota = escenaN2->addEllipse(proj->getX(), proj->getY(), proj->getAncho(), proj->getAlto(),
                                          QPen(QColor(255, 220, 100)), QBrush(QColor(240, 235, 210)));
        itemsFrameN2.append(gota);
        if (indy->puedeVerTrayectorias()) {
            dibujarTrayectoriaPredictiva(escenaN2, itemsFrameN2, proj);
        }
    }
    // 5. Capataz Mayor animado segun su estado de IA
    if (capataz && capataz->estaActiva()) {
        bool capMiraIzq = (capataz->getDireccion() < 0);
        QString claveCap = "cap_parado";
        if (capataz->getEstado() == EstadoCapataz::ATAQUE_A_DISTANCIA) {
            claveCap = ((contadorFrames / 15) % 2 == 0) ? "cap_prepara" : "cap_dispara";
        } else if (capataz->getEstado() == EstadoCapataz::ATAQUE_CERCANO) {
            claveCap = "cap_dispara";
        } else if (std::abs(capataz->getVelX()) > 5.0f) {
            int paso = (contadorFrames / 8) % 3;
            claveCap = (paso == 0) ? "cap_camina1" : (paso == 1 ? "cap_camina2" : "cap_camina3");
        }
        QPixmap pixCap = obtenerSprite(claveCap, capataz->getAncho() + 24, capataz->getAlto() + 18, capMiraIzq);
        if (!pixCap.isNull()) {
            auto *spriteCap = escenaN2->addPixmap(pixCap);
            spriteCap->setPos(capataz->getX() - 12, capataz->getY() - 14);
            itemsFrameN2.append(spriteCap);
        }
    }
    // 6. Cuerda del latigo si Indy esta colgado en pendulo
    if (indy->estaUsandoPendulo()) {
        float indyCentroX = indy->getX() + indy->getAncho() * 0.5f;
        float indyCentroY = indy->getY() + 10.0f;
        const PuntoAnclaje *anclaCercana = nullptr;
        float minDist = 99999.0f;
        for (const PuntoAnclaje &p : nivel2->getPuntosAnclaje()) {
            float d = std::hypot(indyCentroX - p.x, indyCentroY - p.y);
            if (d < minDist) {
                minDist = d;
                anclaCercana = &p;
            }
        }
        if (anclaCercana) {
            auto *cuerda = escenaN2->addLine(anclaCercana->x, anclaCercana->y, indyCentroX, indyCentroY,
                                             QPen(QColor(135, 85, 40), 4));
            itemsFrameN2.append(cuerda);
        }
    }
    // 7. Indiana Jones animado (parado, camina1/2, péndulo izq/centro/der) + Latigazo
    bool indyMiraIzq = (indy->getDireccionMirada() < 0);
    QString claveIndy = "indy_parado";
    if (indy->estaUsandoPendulo()) {
        if (indy->getVelX() < -15.0f) claveIndy = "pendulo_izq";
        else if (indy->getVelX() > 15.0f) claveIndy = "pendulo_der";
        else claveIndy = "pendulo_centro";
    } else if (std::abs(indy->getVelX()) > 5.0f) {
        claveIndy = ((contadorFrames / 8) % 2 == 0) ? "indy_camina1" : "indy_camina2";
    }
    QPixmap pixIndy = obtenerSprite(claveIndy, indy->getAncho() + 24, indy->getAlto() + 16, indyMiraIzq);
    if (!pixIndy.isNull()) {
        auto *spriteIndy = escenaN2->addPixmap(pixIndy);
        spriteIndy->setPos(indy->getX() - 12, indy->getY() - 12);
        itemsFrameN2.append(spriteIndy);
    }
    if (indy->getEstaAtacando()) {
        float xInicio = (indy->getDireccionMirada() == 1) ? (indy->getX() + indy->getAncho()) : indy->getX();
        float xFin = xInicio + (90.0f * indy->getDireccionMirada());
        float yLatigo = indy->getY() + indy->getAlto() * 0.45f;
        auto *latigo = escenaN2->addLine(xInicio, yLatigo, xFin, yLatigo, QPen(QColor(180, 120, 60), 5));
        itemsFrameN2.append(latigo);
    }
}

void MainWindow::configurarAudio() {
    // 1. Musica de fondo en volumen bajo (20%) y en bucle infinito
    musicaFondo = new QSoundEffect(this);
    musicaFondo->setSource(QUrl("qrc:/00. musica_fondo.wav"));
    musicaFondo->setLoopCount(QSoundEffect::Infinite);
    musicaFondo->setVolume(0.20f);

    // 2. Efectos de sonido de eventos
    fxDisparo = new QSoundEffect(this);
    fxDisparo->setSource(QUrl("qrc:/00. sonido_disparo.wav"));
    fxDisparo->setVolume(0.75f);

    fxGolpe = new QSoundEffect(this);
    fxGolpe->setSource(QUrl("qrc:/00. sonido_golpe.wav"));
    fxGolpe->setVolume(0.80f);

    fxItem = new QSoundEffect(this);
    fxItem->setSource(QUrl("qrc:/00. sonido_item.wav"));
    fxItem->setVolume(0.75f);

    fxLatigo = new QSoundEffect(this);
    fxLatigo->setSource(QUrl("qrc:/00. sonido_latigo.wav"));
    fxLatigo->setVolume(0.80f);

    fxPalanca = new QSoundEffect(this);
    fxPalanca->setSource(QUrl("qrc:/00. sonido_palanca.wav"));
    fxPalanca->setVolume(0.85f);
}
