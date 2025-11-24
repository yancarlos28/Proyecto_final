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
    pixmap_lanza  = new QPixmap(":/sprites/caverman_lanza.png");
    pixmap_MamutDanio  = new QPixmap(":/sprites/mamut_herido.png");
    pixmap_MamutMuerto = new QPixmap(":/sprites/mamut_muerto.png");
    // valores iniciales comunes
    filas        = 0;
    columnas     = 0;
    filas_mamut  = 0;
    columnas_mamut = 0;

    // dimensiones por defecto del caverman
    ancho  = 49;
    alto   = 71;

    // dimensiones del mamut
    ancho_mamut = 15+370;
    alto_mamut  = 260;

    // si es mamut, el boundingRect usará el tamaño del mamut
    if (tipoSprite == Tipo::Mamut) {
        ancho = ancho_mamut;
        alto  = alto_mamut;
    }

    estado = Estado::Quieto;
    estadoAnterior = Estado::Quieto;
    estadoMamut = EstadoMamut::Normal;
    dir = Direccion::Derecha;

    timer->start(70);

    connect(timer, &QTimer::timeout,
            this, &sprite::Actualizacion);

    // timer del efecto de daño (solo caverman por ahora)
    timerDanio->setSingleShot(true);
    connect(timerDanio, SIGNAL(timeout()),
            this, SLOT(finDanio()));
}

sprite::Direccion sprite::getDireccion() const { return dir; }

//Modos para cuando recibe daño
void sprite::mostrarDanio()
{
    // guardar en qué estábamos (Quieto, Caminar, Saltar)
    estadoAnterior = estado;

    estado = Estado::Danio;
    columnas = 0;

    // arrancar temporizador de 500 ms
    timerDanio->start(100);
}
void sprite::finDanio()
{
    if (tipoSprite == Tipo::Caverman) {
        // volver al estado anterior del caverman
        estado = estadoAnterior;
        columnas = 0;
    } else if (tipoSprite == Tipo::Mamut) {
        // si el mamut estaba en daño, vuelve a normal
        if (estadoMamut == EstadoMamut::Danio) {
            estadoMamut = EstadoMamut::Normal;
        }
    }
}
void sprite::mostrarDanioMamut()
{

    if (tipoSprite != Tipo::Mamut) {
        return;
    }

    estadoMamut = EstadoMamut::Danio;
    columnas_mamut = 0;
    timerDanio->start(200);       // tiempo de impacto
}
//Modo muerto del mamut
void sprite::matarMamut()
{
    if (tipoSprite != Tipo::Mamut) {
        return;
    }

    estadoMamut = EstadoMamut::Muerto;
    columnas_mamut = 0;
    // aquí NO arrancamos timer, se queda muerto
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
/*void sprite::setVidaBarra(int vida)
{
    // Solo tiene sentido si este sprite es una barra de vida
    if (tipoSprite != Tipo::BarraVida || pixmapVida == nullptr)
        return;

    // Clampear vida entre 0 y 100
    if (vida < 0) vida = 0;
    if (vida > 100) vida = 100;

    // Cada corazón vale 20 puntos de vida (100 / 5)
    // 100 → 0 (fila 0, 5 corazones)
    // 80  → 1 (fila 1, 4 corazones)
    // 60  → 2 (fila 2, 3 corazones)
    // 40  → 3 (fila 3, 2 corazones)
    // 20 o 0 → 4 (fila 4, 1 o 0 corazones según tu imagen)
    int perdida = 100 - vida;
    int indice = perdida / 20;

    if (indice < 0) indice = 0;
    if (indice > 4) indice = 4;

    filaVida = indice;

    // Redibujar
    update();
}
*/
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
        if (estadoMamut == EstadoMamut::Normal) {
            // solo animamos cuando está normal (corriendo)
            columnas_mamut += ancho_mamut;
            if (columnas_mamut >= pixmap_Mamut->width()) {
                columnas_mamut = 0;
            }
        } else {
            // si está herido o muerto, usamos siempre el primer frame
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

        }
        else if (estado == Estado::lanza) {
            const QPixmap *actual = pixmap_lanza;

            if (dir == Direccion::Izquierda) {
                painter->scale(-1, 1);
                painter->drawPixmap(-73/2 * -1 - 73,
                                    -72/2,
                                    *actual);
            } else {
                painter->drawPixmap(-73/2, -72/2,
                                    *actual);
            }

        }
        else {
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
        const QPixmap *actual = nullptr;

        if (estadoMamut == EstadoMamut::Normal) {
            actual = pixmap_Mamut;
        }
        else if (estadoMamut == EstadoMamut::Danio) {
            actual = pixmap_MamutDanio;
        }
        else if (estadoMamut == EstadoMamut::Muerto) {
            actual = pixmap_MamutMuerto;
        }

        int srcX = static_cast<int>(columnas_mamut);
        int srcY = static_cast<int>(filas_mamut); // si solo hay una fila, 0

        if (dir == Direccion::Izquierda) {
            painter->scale(-1, 1);
            painter->drawPixmap(-ancho/2 * -1 - ancho, -alto/2,
                                *actual,
                                srcX, srcY,
                                static_cast<int>(ancho_mamut),
                                static_cast<int>(alto_mamut));
        } else {
            painter->drawPixmap(-ancho/2, -alto/2,
                                *actual,
                                srcX, srcY,
                                static_cast<int>(ancho_mamut),
                                static_cast<int>(alto_mamut));
        }
    }

    painter->restore();
}


