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
#include <QSoundEffect>
#include <memory>
#include <vector>
#include <QMediaPlayer>
#include <QAudioOutput>

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
   void on_pushButtonVolcan_clicked();
   void on_pushButtonMamut_clicked();
   void on_pushButtonSnow_clicked();

private:

    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    enum class EstadoJuego { EnMenu, EnNivel };
    EstadoJuego estadoJuego = EstadoJuego::EnMenu;
    // --- Sonidos ---
    QSoundEffect *sonidoAmbienteVolcan = nullptr;
    QSoundEffect *sonidoGolpeVolcan    = nullptr;
    // --- Sonidos ---
    QMediaPlayer *playerMamut = nullptr;
    QAudioOutput *audioMamut = nullptr;


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
    // Contadores de tiempo
    int segundosJuego = 0;
    int duracionNivel = 0;
    int segundosRestantesNivel = 0;

    //elementos de escena
    void actualizarBarraVida(int vida);
    // Barra de vida de persona y de enemigo
    QGraphicsPixmapItem *barraVidaItem = nullptr;
    QPixmap barraVidaSheet;
    int vidaFrameAncho = 0;
    int vidaFrameAlto  = 0;
    // --- Meta del nivel volcán ---
    QGraphicsPixmapItem *metaVolcanItem = nullptr;
    bool metaVolcanTomada = false;
    bool metaVolcanCompletada = false;
    float volcanInicioX = 0.0f;
    float volcanInicioY = 0.0f;


    QGraphicsPixmapItem *barraVidaEnemigo= nullptr;
    //QPixmap barraVidaSheet;
    //int vidaFracmentoAncho = 0;
    //int vidaFracmentoAlto  = 0;
    // Termómetro (nivel Snowman)
    QPixmap termometroSheet;
    QGraphicsPixmapItem *termometroItem = nullptr;
    int termFrameAncho = 100;
    int termFrameAlto  = 293;


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
    std::vector<QGraphicsPixmapItem*> corazonesSprites;
    void sincronizarCorazonesVolcan();
    bool evaluarColisionCorazonesConJugador();

    //Mamut
    std::vector<QGraphicsPixmapItem*> lanzasSprites;
    void sincronizarLanzasConNivel();
    bool evaluarColisionLanzasConMamut();
    bool evaluarColisionMamutConJugador();
    bool evaluarColisionFlechasConJugador();
    //Soga
    QGraphicsLineItem *sogaItem = nullptr;

    //Snowman
    std::vector<QGraphicsPixmapItem*> bolasNieveSprites;
    void sincronizarBolasNieveConNivel();
    bool evaluarColisionBolasNieveConJugador();
    // Antorcha
    QGraphicsPixmapItem *antorchaItem = nullptr;
    std::vector<QGraphicsPixmapItem*> bolitasFuegoSprites;
    void sincronizarAntorchaConNivel();
    void sincronizarBolitasFuegoConNivel();
    bool evaluarColisionFuegoConSnowman();

    void irAlMenu();
    void iniciarNivel(TipoNivel tipo);

};
#endif // MAINWINDOW_H
