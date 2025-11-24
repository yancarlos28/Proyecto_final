#ifndef SPRITE_H
#define SPRITE_H

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>
#include <QPixmap>
#include <QPainter>

class sprite : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum class Tipo {
        Caverman, Mamut };

    //Para caverman
    explicit sprite(QObject *parent = nullptr, Tipo tipo = Tipo::Caverman);
    QPixmap *pixmap;
    QPixmap *pixmap_danio;
    QPixmap *pixmap_lanza;
    float filas,columnas;
    float ancho;
    float alto;

    //Estados del caverman
    enum class Estado { Quieto, Caminar, Saltar, Danio, lanza };
    Estado estadoAnterior;
    enum class Direccion { Derecha, Izquierda };
    Direccion getDireccion() const;

    /*/ --- Barra de vida ---
    QPixmap *pixmapVida = nullptr;
    int vidaAncho = 0;     // ancho de un frame (530)
    int vidaAlto = 0;      // alto de un frame (600/5 = 120)
    int filaVida = 0;      // 0 = 5 corazones, 1 = 4, ... 4 = 1
*/
    //Reserva para tiempos
    QTimer *timer;
    QTimer *timerDanio;

    //Mamut
    QPixmap *pixmap_Mamut;
    QPixmap *pixmap_MamutDanio;
    QPixmap *pixmap_MamutMuerto;
    float filas_mamut, columnas_mamut;
    float ancho_mamut;
    float alto_mamut;
    enum class EstadoMamut { Normal, Danio, Muerto };
    EstadoMamut estadoMamut = EstadoMamut::Normal;

    //Setter
    void setEstado(Estado e);
    void setDireccion(Direccion d) { dir = d; }
    void mostrarDanio();
    //setter mamut
    void mostrarDanioMamut(); // cambia a sprite de golpeado un momento
    void matarMamut();        // cambia al sprite de muerto
    void setVidaBarra(int vida);



    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:

public slots:

    void Actualizacion();

private slots:

    void finDanio();

private:

    Estado estado = Estado::Quieto;
    Direccion dir = Direccion::Derecha;
    Tipo tipoSprite;
};

#endif // SPRITE_H
