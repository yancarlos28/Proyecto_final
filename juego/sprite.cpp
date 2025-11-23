#include "sprite.h"

//constructor sprite
sprite::sprite(QObject *parent, Tipo tipo)
    : QObject{parent}
    , tipoSprite(tipo)
{
    timer = new QTimer(this);
    timerDanio = new QTimer(this);

    pixmap        = new QPixmap(":/sprites/walk-caverman.png");
    pixmap_danio  = new QPixmap(":/sprites/danio.png");
    pixmap_Mamut  = new QPixmap(":/sprites/mamut_corriendo.png");

    // valores iniciales comunes
    filas        = 0;
    columnas     = 0;
    filas_mamut  = 0;
    columnas_mamut = 0;

    // dimensiones por defecto del caverman
    ancho  = 49;
    alto   = 71;

    // dimensiones del mamut (las guardas también)
    ancho_mamut = 15+370;
    alto_mamut  = 260;

    // si es mamut, el boundingRect usará el tamaño del mamut
    if (tipoSprite == Tipo::Mamut) {
        ancho = ancho_mamut;
        alto  = alto_mamut;
    }

    estado = Estado::Quieto;
    estadoAnterior = Estado::Quieto;
    dir = Direccion::Derecha;

    timer->start(70);

    connect(timer, &QTimer::timeout,
            this, &sprite::Actualizacion);

    // timer del efecto de daño (solo caverman por ahora)
    timerDanio->setSingleShot(true);
    connect(timerDanio, SIGNAL(timeout()),
            this, SLOT(finDanio()));
}

//Modos para cuando recibe daño
void sprite::mostrarDanio()
{
    // guardar en qué estábamos (Quieto, Caminar, Saltar)
    estadoAnterior = estado;

    estado = Estado::Danio;
    columnas = 0;

    // arrancar temporizador de 500 ms
    timerDanio->start(10);
}
void sprite::finDanio()
{
    // volver al estado anterior
    estado = estadoAnterior;
    columnas = 0;
}

void sprite::setEstado(Estado e)
{
    if (e == Estado::Danio) {
        mostrarDanio();
        return;
    }

    estado = e;
    columnas = 0;  // empezar desde el primer frame de esa animación
}
void sprite::Actualizacion()
{
    if (tipoSprite == Tipo::Caverman) {
        // --- ANIMACIÓN CAVERMAN ---
        if (estado == Estado::Quieto) {
            columnas = 0;
        }
        else if (estado == Estado::Caminar) {
            columnas += ancho;
            if (columnas >= 343) {   // ancho total de tu spritesheet
                columnas = 0;
            }
        }
        else if (estado == Estado::Saltar) {
            columnas = 1;
        }
        else if (estado == Estado::Danio) {
            columnas = 0;
        }

    } else { // Tipo::Mamut
        // --- ANIMACIÓN MAMUT ---
        columnas_mamut += ancho_mamut;
        if (columnas_mamut >= pixmap_Mamut->width()) {
            columnas_mamut = 0;
        }
    }

    this->update(-ancho/2,-alto/2, ancho,alto);

}
QRectF sprite::boundingRect() const
{
    return QRectF(-ancho/2,-alto/2, ancho,alto);
}

void sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();

    if (tipoSprite == Tipo::Caverman) {
        // ==== CAVERMAN ====
        if (estado == Estado::Danio) {
            const QPixmap *actual = pixmap_danio;

            if (dir == Direccion::Izquierda) {
                painter->scale(-1, 1);
                painter->drawPixmap(-ancho/2 * -1 - ancho,
                                    -alto/2,
                                    *actual);
            } else {
                painter->drawPixmap(-ancho/2, -alto/2,
                                    *actual);
            }

        } else {
            const QPixmap *actual = pixmap;

            int srcX = static_cast<int>(columnas);
            int srcY = static_cast<int>(filas);

            if (dir == Direccion::Izquierda) {
                painter->scale(-1, 1);
                painter->drawPixmap(-ancho/2 * -1 - ancho,-alto/2,*actual,srcX, srcY,
                                    static_cast<int>(ancho),
                                    static_cast<int>(alto));
            } else {
                painter->drawPixmap(-ancho/2, -alto/2,*actual,srcX, srcY,
                                    static_cast<int>(ancho),
                                    static_cast<int>(alto));
            }
        }

    } else {
        // ==== MAMUT ====
        const QPixmap *actual = pixmap_Mamut;

        int srcX = static_cast<int>(columnas_mamut);
        int srcY = static_cast<int>(filas_mamut); // si solo hay una fila, 0

        if (dir == Direccion::Izquierda) {
            painter->scale(-1, 1);
            painter->drawPixmap(-ancho/2 * -1 - ancho,-alto/2,*actual,srcX, srcY,
                                static_cast<int>(ancho_mamut),
                                static_cast<int>(alto_mamut));
        } else {
            painter->drawPixmap(-ancho/2, -alto/2,*actual,srcX, srcY,
                                static_cast<int>(ancho_mamut),
                                static_cast<int>(alto_mamut));
        }
    }

    painter->restore();
}


