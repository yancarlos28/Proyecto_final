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
    float filas,columnas;
    float ancho;
    float alto;

    //Estados del caverman
    enum class Estado { Quieto, Caminar, Saltar, Danio };
    Estado estadoAnterior;
    enum class Direccion { Derecha, Izquierda };


    //Reserva para tiempos
    QTimer *timer;
    QTimer *timerDanio;

    //Mamut
    QPixmap *pixmap_Mamut;
    float filas_mamut, columnas_mamut;
    float ancho_mamut;
    float alto_mamut;


    //Setter
    void setEstado(Estado e);
    void setDireccion(Direccion d) { dir = d; }
    void mostrarDanio();


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
