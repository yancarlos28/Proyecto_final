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
        Caverman, Mamut, BarraVida, Snowman };

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

    // --- Barra de vida ---
    QPixmap *pixmapVida = nullptr;
    int vidaAncho = 0;
    int vidaAlto = 0;
    int filaVida = 0;

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

    //Snowman
    QPixmap *pixmap_Snowman=nullptr;
    QPixmap *pixmap_SnowmanDanio;
    QPixmap *pixmap_SnowmanMuerto;
    float filas_Snowman, columnas_Snowman;
    float ancho_Snowman;
    float alto_Snowman;

    // Dimensiones y columnas para el sheet de daño (420x71, 5 frames)
    float columnas_SnowmanDanio = 0.0f;
    float ancho_SnowmanDanio = 0.0f;
    float alto_SnowmanDanio = 0.0f;
    enum class EstadoSnowman { Normal, Danio, Atacar, Muerto };
    EstadoSnowman estadoSnowman = EstadoSnowman::Normal;

    void mostrarDanioSnowman();  // muestra solo el primer frame de la tira 420x71
    void matarSnowman();         // reproduce todos los frames y se queda en el último


    //Setter
    void setEstado(Estado e);
    void setDireccion(Direccion d) { dir = d; }
    void mostrarDanio();

    //setter mamut
    void mostrarDanioMamut(); // cambia a sprite de golpeado un momento
    void matarMamut();        // cambia al sprite de muerto
    void setVidaBarra(int vida);


    //Pintar los objetos
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
