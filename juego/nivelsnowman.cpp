#include "nivelsnowman.h"

#include "nivelsnowman.h"
#include "modelomovimiento.h"
#include <QDebug>
#include <cmath>

NivelSnowman::NivelSnowman()
{
    // 1) Posición inicial del caverman
    //    Vista frontal: lo ponemos abajo, centrado
    jugador.setPos(800, 900); // ajusta según alto de tu escena

    // 2) Posición inicial del Snowman
    jefeSnow.setPos(768, 300);

    // 3) Arma para bolas parabólicas
    //    Modelo con gravedad hacia abajo
    ModeloMovimiento* modeloParabola = new ModeloMovimiento(9.8);
    armaParabolica = new Arma(1, 250, modeloParabola);

    // 4) Estado de patrones
    patronActual = PatronAtaque::Parabolico;
    tiempoDesdeUltimoDisparo = 0.0;
    intervaloDisparo = 1.2;         // dispara cada 1.2 s

    tiempoCambioPatron = 0.0;
    intervaloCambioPatron = 3.0;    // cambia de patrón cada 6 s aprox
}

NivelSnowman::~NivelSnowman()
{
    // Liberar proyectiles parabólicos
    for (proyectil* p : bolasParabolicas) {
        delete p;
    }
    bolasParabolicas.clear();

    // Liberar arma
    delete armaParabolica;
}

// ---------- DISPAROS ----------

void NivelSnowman::dispararParabolicoAlJugador()
{
    // Disparo 1 bola de nieve hacia la posición actual del jugador
    float xSnow = jefeSnow.getX();
    float ySnow = jefeSnow.getY();

    float xJug = jugador.getX();
    float yJug = jugador.getY();

    float dx = xJug - xSnow; //distancia que los separa(en x)
    float dy = yJug - ySnow; //distancia que los separa(en y)

    // Normalizar dirección
    float mag = std::sqrt(dx*dx + dy*dy);
    if (mag < 0.001f) return;
    dx /= mag;
    dy /= mag;

    // Crear proyectil con el arma parabólica
    proyectil* bola = armaParabolica->crearProyectil(xSnow, ySnow, dx, dy);
    bolasParabolicas.push_back(bola);

    qDebug() << "[Snowman] Disparo parabólico hacia el jugador";
}

void NivelSnowman::dispararRafagaOscilante()
{
    // Creamos varias bolas que se mueven en X con trayectoria sinusoidal en Y
    float xSnow = jefeSnow.getX();
    float ySnow = jefeSnow.getY();

    // Decidir dirección base según dónde esté el jugador
    float xJug = jugador.getX();
    float dirX = (xJug >= xSnow) ? 1.0 : -1.0;

    int cantidad = 1;          // 1 bolas en la ráfaga
    float separacion = 50.0;  // pequeña separación vertical

    for (int i = 0; i < cantidad; ++i) {
        BolaNieveOscilante bola;
        bola.x = xSnow;
        bola.y = ySnow - i * separacion;

        bola.velocidadX = 200.0f * dirX; // rápida en X
        bola.velocidadY = 200.0f;

        bola.amplitud = 60.0;   // cuánto sube y baja
        bola.frecuencia = 4.0;  // qué tan rápido oscila
        bola.fase = 0.0f;
        bola.yCentro = bola.y;   // línea central para la oscilación

        bolasOscilantes.push_back(bola);
    }

    qDebug() << "[Snowman] Ráfaga oscilante";
}

// ---------- ACTUALIZACIONES DE PROYECTILES ----------

void NivelSnowman::actualizarBolasParabolicas(float dt)
{
    int i = 0;
    while (i < bolasParabolicas.size()) {
        proyectil* p = bolasParabolicas[i];

        // Actualizar usando el ModeloMovimiento (parabólico)
        p->actualizar(dt);

        float x = p->getX();
        float y = p->getY();

        // Límite sencillo de la escena (ajusta a tu tamaño real)
        if (x < 0.0f || x > 1536.0f || y < 0.0f || y > 900.0f) {
            delete p;
            bolasParabolicas.erase(bolasParabolicas.begin() + i);
        }
        else {
            i++;
        }
    }
}

void NivelSnowman::actualizarBolasOscilantes(float dt)
{
    int i = 0;
    while (i < bolasOscilantes.size()) {
        BolaNieveOscilante &b = bolasOscilantes[i];

        b.x += b.velocidadX * dt;      // movimiento horizontal
        b.yCentro += b.velocidadY * dt; // el centro se desplaza hacia abajo

        b.fase += b.frecuencia * dt;

        // Y se oscilante alrededor de ese centro que va cayendo
        b.y = b.yCentro + b.amplitud * std::sin(b.fase);


        // Eliminar si sale de pantalla
        if (b.x < 0.0f || b.x > 1536.0f) {
            bolasOscilantes.erase(bolasOscilantes.begin() + i);
        } else {
            ++i;
        }
    }
}

// ---------- LOOP PRINCIPAL DEL NIVEL ----------

void NivelSnowman::actualizar(float dt)
{
    // 1) El Snowman puede tener pequeña animación/IA propia
    jefeSnow.actuar(dt);

    // 2) Control de patrones y disparos
    tiempoDesdeUltimoDisparo += dt;
    tiempoCambioPatron        += dt;

    // Cambiar patrón cada cierto tiempo
    if (tiempoCambioPatron >= intervaloCambioPatron) {
        tiempoCambioPatron = 0.0;
        if (patronActual == PatronAtaque::Parabolico) {
            patronActual = PatronAtaque::Oscilatorio;
        } else {
            patronActual = PatronAtaque::Parabolico;
        }
        qDebug() << "[Snowman] Cambio de patrón a"
                 << (patronActual == PatronAtaque::Parabolico ? "Parabólico" : "Oscilatorio");
    }

    // Disparar según el patrón actual
    if (tiempoDesdeUltimoDisparo >= intervaloDisparo) {
        tiempoDesdeUltimoDisparo = 0.0f;

        if (patronActual == PatronAtaque::Parabolico) {
            dispararParabolicoAlJugador();
        } else {
            dispararRafagaOscilante();
        }
    }

    // 3) Actualizar proyectiles
    actualizarBolasParabolicas(dt);
    actualizarBolasOscilantes(dt);

    // 4) Más adelante: aquí irán las colisiones (bolas ↔ jugador)
}


Snowman &NivelSnowman::getSnowman() { return jefeSnow; }

const Snowman &NivelSnowman::getSnowman() const { return jefeSnow; }

const vector<proyectil *> &NivelSnowman::getBolasParabolicas() const { return bolasParabolicas; }

const vector<BolaNieveOscilante> &NivelSnowman::getBolasOscilantes() const { return bolasOscilantes; }
void NivelSnowman::eliminarBolaPorIndice(size_t i)
{
    if (i < bolasParabolicas.size()) {
        // Está en el vector de parabólicas
        delete bolasParabolicas[i];
        bolasParabolicas.erase(bolasParabolicas.begin() + i);
    }
    else {
        // Está en el vector de oscilantes
        size_t j = i - bolasParabolicas.size();
        if (j < bolasOscilantes.size()) {
            bolasOscilantes.erase(bolasOscilantes.begin() + j);
        }
    }
}
