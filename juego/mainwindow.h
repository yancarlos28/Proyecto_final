#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QGraphicsScene"
#include "QGraphicsItem"
#include "QGraphicsView"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>
#include <vector>

#include "sprite.h"
#include "nivel.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:

   void actualizarJuego();
   void actualizarTiempo();
   void mostrarMenu();

   //void on_pushButtonVolcan();
   //void on_pushButtonMamut();
   //void on_pushButtonSnow();

   void on_pushButtonVolcan_clicked();

   void on_pushButtonMamut_clicked();

   void on_pushButtonSnow_clicked();

private:

    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    enum class EstadoJuego { EnMenu, EnNivel };
    EstadoJuego estadoJuego = EstadoJuego::EnMenu;

    //logica del nivel
    std::unique_ptr<Nivel> nivel;

    //--------sprite del personajes:caverman,mamut----------
    sprite *cavermanSprite;
    sprite *mamutSprite;
    sprite *snowmanSprite=nullptr;


    //movimientos de teclado para personaje principal
    bool moviendoArriba = false;
    bool moviendoAbajo = false;
    bool moviendoIzquierda = false;
    bool moviendoDerecha = false;

    //Tiempos
    QTimer *timerJuego=nullptr;
    QTimer *timerTiempo=nullptr;
    // Contadores de tiempo ---
    int segundosJuego = 0;
    int duracionNivel = 0;
    int segundosRestantesNivel = 0;

    //elementos de escena
    void actualizarBarraVida(int vida);
    // === Barra de vida ===
    QGraphicsPixmapItem *barraVidaItem = nullptr;
    QPixmap barraVidaSheet;
    int vidaFrameAncho = 0;
    int vidaFrameAlto  = 0;

    //Niveles
    enum class TipoNivel { Volcan, Mamut, JefeSnow };
    TipoNivel tipoNivelActual = TipoNivel::Volcan;
    void cargarNivel(TipoNivel tipo);
    void limpiarSpritesNivel();
    void cambiarAlSiguienteNivel();

    //Volcan
    std::vector<QGraphicsPixmapItem*> bolasSprites;
    void sincronizarBolasConNivel();
    bool evaluarColisionBolasConJugador();

    //Mamut
    std::vector<QGraphicsPixmapItem*> lanzasSprites;
    void sincronizarLanzasConNivel();
    bool evaluarColisionLanzasConMamut();
    bool evaluarColisionMamutConJugador();
    //Soga
    QGraphicsLineItem *sogaItem = nullptr;

    //Hombre nieve
    std::vector<QGraphicsPixmapItem*> bolasNieveSprites;
    void sincronizarBolasNieveConNivel();
    bool evaluarColisionBolasNieveConJugador();

    void irAlMenu();
    void iniciarNivel(TipoNivel tipo);



};
#endif // MAINWINDOW_H
