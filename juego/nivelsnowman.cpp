#include "nivelsnowman.h"

#include "nivelsnowman.h"
#include "modelomovimiento.h"
#include <QDebug>
#include <cmath>

NivelSnowman::NivelSnowman()
{
    // 1) Posición inicial del caverman
    jugador.setPos(800, 900);

    // 2) Posición inicial del Snowman
    jefeSnow.setPos(768, 300);

    // 3) Arma para bolas parabólicas
    //    Modelo con gravedad hacia abajo
    ModeloMovimiento* modeloParabola = new ModeloMovimiento(9.8);
    armaParabolica = new Arma(1, 250, modeloParabola);

    // 4) Estado de patrones
    patronActual = PatronAtaque::Parabolico;
    tiempoDesdeUltimoDisparo = 0.0;
    intervaloDisparo = 1.2;

    tiempoCambioPatron = 0.0;

    // Antorcha
    antorcha.activa        = false;
    antorcha.explotada     = false;
    antorcha.x             = 0.0f;
    antorcha.y             = 0.0f;
    antorcha.velocidadY    = -250.0;   // sube
    antorcha.angulo        = 0.0;
    antorcha.velAngular    = 360.0;    // 1 vuelta por segundo

    alturaExplosionAntorcha = jefeSnow.getY() + 200.0;

    // Para arreglo de la antorcha
    antorcha.yInicio            = 0.0;
    antorcha.distanciaExplosion = 0.0;

    // Temperatura del nivel
    tempMax = 0.0;        // empieza a 0°C
    tempMin = -30.0;      // mínimo -30°C
    temperaturaActual = tempMax;
    velocidadEnfriamiento = 0.3;   // baja ~0.3 °C por segundo

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
    if (mag < 0.001) return;
    dx /= mag;
    dy /= mag;

    // Crear proyectil con el arma parabólica
    proyectil* bola = armaParabolica->crearProyectil(xSnow, ySnow, dx, dy);
    bolasParabolicas.push_back(bola);

    //qDebug() << "[Snowman] Disparo parabólico hacia el jugador";
}

void NivelSnowman::dispararRafagaOscilante()
{
    float xSnow = jefeSnow.getX();
    float ySnow = jefeSnow.getY();

    float xJug = jugador.getX();
    float dirX = (xJug >= xSnow) ? 1.0 : -1.0;
    //Cantidad para ráfagas
    int cantidad = 1;
    float separacion = 50.0;

    for (int i = 0; i < cantidad; i++) {
        BolaNieveOscilante bola;
        bola.x = xSnow;
        bola.y = ySnow - i * separacion;

        bola.velocidadX = 200.0 * dirX;
        bola.velocidadY = 200.0;

        bola.amplitud = 60.0;
        bola.frecuencia = 4.0;
        bola.fase = 0.0;
        bola.yCentro = bola.y;

        bolasOscilantes.push_back(bola);
    }

    //qDebug() << "[Snowman] Ráfaga oscilante";
}
//Lanzamiento de antorcha
void NivelSnowman::lanzarAntorcha()
{
    if (antorcha.activa && !antorcha.explotada) return;

    float xJug = jugador.getX();
    float yJug = jugador.getY();

    antorcha.x = xJug -130.0;
    antorcha.y = yJug - 400.0;

    antorcha.yInicio     = antorcha.y;   // por si luego quieres uso distancia

    antorcha.activa      = true;
    antorcha.explotada   = false;
    antorcha.angulo      = 0.0;
    alturaExplosionAntorcha = jefeSnow.getY() + 20.0;
}


// Actualizar proyectiles

void NivelSnowman::actualizarBolasParabolicas(float dt)
{
    int i = 0;
    while (i < bolasParabolicas.size()) {
        proyectil* p = bolasParabolicas[i];

        // Actualizar usando el m. parabólico
        p->actualizar(dt);

        float x = p->getX();
        float y = p->getY();

        // Límite sencillo de la escena
        if (x < 0.0f || x > 1536.0 || y < 0.0f || y > 900.0f) {
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

        b.x += b.velocidadX * dt;
        b.yCentro += b.velocidadY * dt;

        b.fase += b.frecuencia * dt;

        // Oscila por el centro que va cayendo
        b.y = b.yCentro + b.amplitud * std::sin(b.fase);


        // Eliminar si sale de vista
        if (b.x < 0.0 || b.x > 1536.0) {
            bolasOscilantes.erase(bolasOscilantes.begin() + i);
        } else {
            i++;
        }
    }
}

// Actualización principal del nivel

void NivelSnowman::actualizar(float dt)
{
    // 1) Actualizar temperatura del entorno
    temperaturaActual -= velocidadEnfriamiento * dt;
    if (temperaturaActual < tempMin)
        temperaturaActual = tempMin;

    // 2) Actua agente inteligente
    jefeSnow.percibirTemperatura(temperaturaActual);
    jefeSnow.actuar(dt);  // Decide fase y movimiento

    // 3) Control de disparos
    tiempoDesdeUltimoDisparo += dt;

    // Elegir patrón según la fase del Snowman
    int fase = jefeSnow.getFaseAtaque();

    if (fase == 1 || fase == 2) {
        patronActual = PatronAtaque::Parabolico; //dirección rectilínea al jugador
    } else {
        patronActual = PatronAtaque::Oscilatorio;  // Fase 3: Ondulatorio
    }

    // Disparar según el patrón actual
    if (tiempoDesdeUltimoDisparo >= intervaloDisparo) {
        tiempoDesdeUltimoDisparo = 0.0;

        if (patronActual == PatronAtaque::Parabolico) {
            dispararParabolicoAlJugador();
        } else {
            dispararRafagaOscilante();
        }
    }

    // 4) Actualizar proyectiles
    actualizarBolasParabolicas(dt);
    actualizarBolasOscilantes(dt);

    // 5) Colisiones se manejan en MainWindow (ya lo tienes)
    actualizarAntorchaYFuego(dt);
}

void NivelSnowman::actualizarAntorchaYFuego(float dt)
{
    // Antorcha en subida
    if (antorcha.activa && !antorcha.explotada) {

        // Movimiento vertical
        antorcha.y += antorcha.velocidadY * dt;

        // Rotación
        antorcha.angulo += antorcha.velAngular * dt;
        if (antorcha.angulo >= 360.0) antorcha.angulo -= 360.0;
        if (antorcha.angulo < 0.0)    antorcha.angulo += 360.0;

        // Altura de explosión

        float distanciaRecorrida = antorcha.yInicio - antorcha.y;
        //Explotar
        if (distanciaRecorrida >= 500.0) {
            antorcha.explotada = true;
            crearFragmentosDesdeAntorcha();
        }

        if (antorcha.y < -100.0) {
            antorcha.activa = false;
        }
    }

    // Bolas de fuego
    int i = 0;
    while (i < static_cast<int>(bolitasFuego.size())) {
        BolitaFuego &b = bolitasFuego[i];

        b.x += b.velX * dt;
        b.y += b.velY * dt;

        // "Gravedad" hacia abajo
        b.velY += 200.0 * dt;

        if (b.x < 0.0f || b.x > 1920.0 || b.y < 0.0 || b.y > 1200.0) {
            bolitasFuego.erase(bolitasFuego.begin() + i);
        } else {
            ++i;
        }
    }
}


Snowman &NivelSnowman::getSnowman() {
    return jefeSnow; }

const Snowman &NivelSnowman::getSnowman() const {
    return jefeSnow; }

const vector<proyectil *> &NivelSnowman::getBolasParabolicas() const {
    return bolasParabolicas; }

const vector<BolaNieveOscilante> &NivelSnowman::getBolasOscilantes() const {
    return bolasOscilantes; }

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
const AntorchaJugador& NivelSnowman::getAntorcha() const
{
    return antorcha;
}

bool NivelSnowman::hayAntorchaActiva() const
{
    return antorcha.activa && !antorcha.explotada;
}

float NivelSnowman::getTemperaturaActual() const { return temperaturaActual; }

const std::vector<BolitaFuego>& NivelSnowman::getBolitasFuego() const
{
    return bolitasFuego;
}

void NivelSnowman::eliminarBolitaFuegoEnIndice(size_t i)
{
    if (i < bolitasFuego.size()) {
        bolitasFuego.erase(bolitasFuego.begin() + i);
    }
}
void NivelSnowman::crearFragmentosDesdeAntorcha()
{
    const int N = 5;
    const float velocidadBase = 250.0;

    float anguloBase = -M_PI / 2.0;      // Hacia arriba
    float separacion = M_PI / 12.0;      // Abanico

    for (int i = 0; i < N; ++i) {
        BolitaFuego b;
        b.x = antorcha.x+130;    // "Misma posición lógica" de la antorcha
        b.y = antorcha.y+420;

        float offset = (i - (N - 1) / 2.0f) * separacion;
        float ang = anguloBase + offset;

        b.velX = velocidadBase * std::cos(ang);
        b.velY = velocidadBase * std::sin(ang);

        bolitasFuego.push_back(b);
    }

    // opcional: Se desactiva la antorcha para que ya no se dibuje como proyectil
    antorcha.activa = false;

}

