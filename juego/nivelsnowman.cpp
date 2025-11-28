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

    // ---------- ANTORCHA DEL JUGADOR ----------
    antorcha.activa        = false;
    antorcha.explotada     = false;
    antorcha.x             = 0.0f;
    antorcha.y             = 0.0f;
    antorcha.velocidadY    = -250.0f;   // sube
    antorcha.angulo        = 0.0f;
    antorcha.velAngular    = 360.0f;    // 1 vuelta por segundo

    // No nos complicamos: la antorcha explota SIEMPRE
    // cerca del Snowman, un poquito por debajo
    alturaExplosionAntorcha = jefeSnow.getY() + 200.0f;

    // Estos dos ya no son críticos, pero los dejamos con algo coherente
    antorcha.yInicio            = 0.0f;
    antorcha.distanciaExplosion = 0.0f;  // NO la vamos a usar en la condición
    // --- NUEVO: temperatura del nivel ---
    tempMax = 0.0f;        // empieza a 0°C
    tempMin = -30.0f;      // mínimo -30°C
    temperaturaActual = tempMax;
    velocidadEnfriamiento = 0.3f;   // baja ~0.3 °C por segundo (ajusta)

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
void NivelSnowman::lanzarAntorcha()
{
    // Solo 1 antorcha en vuelo a la vez
    if (antorcha.activa && !antorcha.explotada) return;

    float xJug = jugador.getX();
    float yJug = jugador.getY();

    // IMPORTANTE:
    // Vamos a tratar (antorcha.x, antorcha.y) como CENTRO de la antorcha,
    // porque en MainWindow la dibujas centrada.
    //
    // xJug, yJug son la posición lógica del caverman (arriba-izquierda del sprite),
    // así que la antorcha la "pegamos" un poco encima del jugador:

    antorcha.x = xJug -130.0f;   // un poquito a la derecha del caverman
    antorcha.y = yJug - 400.0f;   // por encima del caverman

    antorcha.yInicio     = antorcha.y;   // por si luego quieres usar distancia
    antorcha.velocidadY  = -250.0f;      // hacia arriba

    antorcha.activa      = true;
    antorcha.explotada   = false;
    antorcha.angulo      = 0.0f;
    alturaExplosionAntorcha = jefeSnow.getY() + 20.0f; // +20 para un poco debajo del snowman
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
    // 1) Actualizar temperatura del entorno
    temperaturaActual -= velocidadEnfriamiento * dt;
    if (temperaturaActual < tempMin)
        temperaturaActual = tempMin;

    // 2) Pasar temperatura al jefe y dejar que actúe (IA)
    jefeSnow.percibirTemperatura(temperaturaActual);
    jefeSnow.actuar(dt);  // aquí decide fase y movimiento

    // 3) Control de disparos
    tiempoDesdeUltimoDisparo += dt;

    // Elegir patrón según la fase del Snowman
    int fase = jefeSnow.getFaseAtaque();

    if (fase == 1 || fase == 2) {
        patronActual = PatronAtaque::Parabolico;   // Fase 1 y 2: disparo "normal"
    } else { // fase 3
        patronActual = PatronAtaque::Oscilatorio;  // Fase 3: ondulatorio
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

    // 4) Actualizar proyectiles
    actualizarBolasParabolicas(dt);
    actualizarBolasOscilantes(dt);

    // 5) Colisiones se manejan en MainWindow (ya lo tienes)
    actualizarAntorchaYFuego(dt);
}

void NivelSnowman::actualizarAntorchaYFuego(float dt)
{
    // --- ANTORCHA SUBIENDO ---
    if (antorcha.activa && !antorcha.explotada) {

        // Movimiento vertical
        antorcha.y += antorcha.velocidadY * dt;

        // Rotación
        antorcha.angulo += antorcha.velAngular * dt;
        if (antorcha.angulo >= 360.0f) antorcha.angulo -= 360.0f;
        if (antorcha.angulo < 0.0f)    antorcha.angulo += 360.0f;

        // ALTURA DE EXPLOSIÓN: cerca del Snowman
        // Recuerda: en Qt, Y más pequeño = más arriba.
        float distanciaRecorrida = antorcha.yInicio - antorcha.y;  // cuánto ha subido
        if (distanciaRecorrida >= 500.0f) {  // <-- ¡EXPLOSA DESPUÉS DE SUBIR 200px!
            antorcha.explotada = true;
            crearFragmentosDesdeAntorcha();
            qDebug() << "¡EXPLOSIÓN! Distancia recorrida:" << distanciaRecorrida;
        }
        /*if (distanciaRecorrida >= 200.0f && distanciaRecorrida <= 500)
        {
            crearFragmentosDesdeAntorcha();
            qDebug() << "¡EXPLOSIÓN! Distancia recorrida:" << distanciaRecorrida;
        }*/


        // SEGURIDAD: solo se borra si se sale por arriba de la pantalla
        // (esto es por si, por alguna razón, no explotó)
        if (antorcha.y < -100.0) {
            antorcha.activa = false;
        }
    }

    // --- BOLITAS DE FUEGO ---
    int i = 0;
    while (i < static_cast<int>(bolitasFuego.size())) {
        BolitaFuego &b = bolitasFuego[i];

        b.x += b.velX * dt;
        b.y += b.velY * dt;

        // pequeña "gravedad" hacia abajo
        b.velY += 200.0f * dt;

        if (b.x < 0.0f || b.x > 1920.0f || b.y < 0.0f || b.y > 1200.0f) {
            bolitasFuego.erase(bolitasFuego.begin() + i);
        } else {
            ++i;
        }
    }
    qDebug() << "Antorcha Y:" << antorcha.y << "Explosion Y:" << alturaExplosionAntorcha;
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
const AntorchaJugador& NivelSnowman::getAntorcha() const
{
    return antorcha;
}

bool NivelSnowman::hayAntorchaActiva() const
{
    return antorcha.activa && !antorcha.explotada;
}

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
    const float velocidadBase = 250.0f;

    float anguloBase = -M_PI / 2.0f;      // hacia arriba
    float separacion = M_PI / 12.0f;      // abanico

    for (int i = 0; i < N; ++i) {
        BolitaFuego b;
        b.x = antorcha.x+130;    // MISMA posición lógica de la antorcha
        b.y = antorcha.y+420;

        float offset = (i - (N - 1) / 2.0f) * separacion;
        float ang = anguloBase + offset;

        b.velX = velocidadBase * std::cos(ang);
        b.velY = velocidadBase * std::sin(ang);

        bolitasFuego.push_back(b);
    }

    // opcional: desactivamos la antorcha para que ya no se dibuje como proyectil
    antorcha.activa = false;
    qDebug() << "Antorcha desactivada";
}

