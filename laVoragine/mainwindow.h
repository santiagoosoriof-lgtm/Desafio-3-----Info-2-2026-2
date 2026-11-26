#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QTimer>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QHash>
#include <QSet>
#include <QList>
#include "GestorNivel1.h"
#include "GestorNivel2.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_btnNormal_clicked();
    void on_btnDificil_clicked();
    void on_btnReintentar_clicked();
    void on_btnVolverMenu_clicked();
    void actualizarJuego(); // Bucle principal ~60 FPS

private:
    Ui::MainWindow *ui;
    QTimer *timerJuego;

    // Escenas gráficas (1536 x 1024)
    QGraphicsScene *escenaN1;
    QGraphicsScene *escenaN2;

    // Fondos permanentes (Z-Index -10)
    QGraphicsPixmapItem *fondoN1_A;
    QGraphicsPixmapItem *fondoN1_B;
    QGraphicsPixmapItem *fondoN2;
    float scrollRioY;

    // Caché de sprites en memoria (búsqueda O(1) sin lecturas de disco)
    QHash<QString, QPixmap> sprites;
    uint32_t contadorFrames;
    int dirBalsaX; // -1 izq, 0 frente, 1 der

    // Listas de ítems dinámicos por frame
    QList<QGraphicsItem*> itemsFrameN1;
    QList<QGraphicsItem*> itemsFrameN2;

    // Capas lógicas
    GestorNivel1 *nivel1;
    GestorNivel2 *nivel2;
    Dificultad dificultadActual;
    int nivelActivo; // 0 = Menú/Final, 1 = Nivel 1, 2 = Nivel 2

    QSet<int> teclasPresionadas;

    // Métodos internos
    void cargarFondos();
    void cargarSprites();
    QPixmap obtenerSprite(const QString &clave, int ancho, int alto, bool voltearHorizontal = false);

    void iniciarNivel1();
    void iniciarNivel2();
    void mostrarPantallaFinal(const QString &mensaje);

    void limpiarItemsFrame(QGraphicsScene *escena, QList<QGraphicsItem*> &lista);
    void dibujarTrayectoriaPredictiva(QGraphicsScene *escena, QList<QGraphicsItem*> &lista, const Proyectil *proj);
    void dibujarNivel1();
    void dibujarNivel2();
};

#endif // MAINWINDOW_H
