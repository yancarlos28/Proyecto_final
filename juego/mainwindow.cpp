#include "mainwindow.h"
#include "nivelmamut.h"
#include "nivelvolcan.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , nivel(nullptr)
    , cavermanSprite(new sprite(this))
    , mamutSprite(nullptr)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    // Configurar la vista una sola vez
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, 1536, 1024);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setFixedSize(1536, 1024);
    this->setFixedSize(ui->graphicsView->width() + 10,
                       ui->graphicsView->height() + 20);

    // Sprite del jugador
    scene->addItem(cavermanSprite);
    cavermanSprite->setScale(1.4);
    cavermanSprite->setZValue(10);

    // Cargar el primer nivel
    cargarNivel(TipoNivel::Mamut);

    // Timer del juego
    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timerJuego->start(16);
    // --- Timer para segundos ---
    timerTiempo = new QTimer(this);
    connect(timerTiempo, &QTimer::timeout, this, &MainWindow::actualizarTiempo);
    timerTiempo->start(1000);
}





MainWindow::~MainWindow()
{
    delete ui;
}


//Teclado para el caverman
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }
    bool teclaMovimiento = false;
    caverman &jug = nivel->getJugador();

    if (event->key() == Qt::Key_A) {
        moviendoIzquierda = true;
        teclaMovimiento = true;
        if (cavermanSprite)
            cavermanSprite->setDireccion(sprite::Direccion::Izquierda);

    } else if (event->key() == Qt::Key_D) {
        moviendoDerecha = true;
        teclaMovimiento = true;
        if (cavermanSprite)
            cavermanSprite->setDireccion(sprite::Direccion::Derecha);

    } else if (event->key() == Qt::Key_Space) {
        jug.iniciarSalto();

        if (cavermanSprite) {
            cavermanSprite->setEstado(sprite::Estado::Saltar);
        }
        return;

    } else {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (teclaMovimiento && cavermanSprite) {
        cavermanSprite->setEstado(sprite::Estado::Caminar);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }
    if (event->key() == Qt::Key_A) {
        moviendoIzquierda = false;
    }
    else if (event->key() == Qt::Key_D) {
        moviendoDerecha = false;
    }

    if (!moviendoIzquierda && !moviendoDerecha)
    {
        if (cavermanSprite)
            cavermanSprite->setEstado(sprite::Estado::Quieto);
    }

    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::limpiarSpritesNivel()
{
    // Sprites de rocas del volcan
    for (QGraphicsPixmapItem *item : bolasSprites)
    {
        scene->removeItem(item);
        delete item;
    }
    bolasSprites.clear();

    // Sprite del mamut
    if (mamutSprite) {
        scene->removeItem(mamutSprite);
        delete mamutSprite;
        mamutSprite = nullptr;
    }
}

void MainWindow::actualizarTiempo()
{
    // 1) Cronómetro global del juego
    segundosJuego++;
    if (ui->lblTiempoJuego) {
        ui->lblTiempoJuego->setText(
            QString("Tiempo: %1 s").arg(segundosJuego)
            );
    }

    // 2) Temporizador del nivel actual
    if (duracionNivel > 0 && segundosRestantesNivel > 0) {
        segundosRestantesNivel--;

        ui->lblTemporizadorNivel->setText(
            QString("Restante: %1 s").arg(segundosRestantesNivel)
            );

        // TRIGGER: cuando se acaba el tiempo DEL VOLCÁN
        if (segundosRestantesNivel == 0 &&
            tipoNivelActual == TipoNivel::Volcan)
        {
            // pasamos al siguiente nivel
            cambiarAlSiguienteNivel();
        }
    }
}

void MainWindow::cargarNivel(TipoNivel tipo)
{

    limpiarSpritesNivel();

    // 2) Crear el nuevo nivel lógico
    switch (tipo) {
    case TipoNivel::Volcan:
    {
        nivel = std::make_unique<NivelVolcan>();
        scene->setSceneRect(0, 0, 1536, 1024);

        QImage img("C:/Users/Jean/OneDrive/Documentos/INFORMATICA_2/Laboratorio/game/escenas/volcan.png");
        QImage imgEscalada = img.scaled(1536, 1024,
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // Volcán: nivel por tiempo (ejemplo 40 s)
        duracionNivel =5;
        break;
    }

    case TipoNivel::Mamut:
    {
        nivel = std::make_unique<NivelMamut>();

        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());

        float anchoMundo = nivelMamut->getAnchoMundo();
        scene->setSceneRect(0, 0, anchoMundo, 800);

        QImage img(":/escenas/bosque.jpg");
        QImage imgEscalada = img.scaled(static_cast<int>(anchoMundo),800 ,
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // Sprite del mamut
        mamutSprite = new sprite(this, sprite::Tipo::Mamut);
        scene->addItem(mamutSprite);
        mamutSprite->setScale(1);
        mamutSprite->setZValue(9);

        mamut &boss = nivelMamut->getMamut();
        mamutSprite->setPos(boss.getX(), boss.getY());

        // Este nivel NO está limitado por tiempo (por ahora)
        duracionNivel = 0;
        break;
    }

    case TipoNivel::JefeSnow:
    {
        // MÁS ADELANTE: implementar nivel de jefe
        duracionNivel = 0;
        break;
    }
    }

    tipoNivelActual = tipo;
    // 3) Reiniciar temporizador visual del nivel
    segundosRestantesNivel = duracionNivel;

    if (duracionNivel > 0) {
        ui->lblTemporizadorNivel->setVisible(true);
        ui->lblTemporizadorNivel->setText(
            QString("Restante: %1 s").arg(segundosRestantesNivel));
    } else {
        ui->lblTemporizadorNivel->setVisible(false);
    }


    // 4) Reposicionar el sprite según la posición lógica del jugador
    caverman &j = nivel->getJugador();
    cavermanSprite->setPos(j.getX(), j.getY());

    // Centrar vista si es el nivel con scroll
    if (tipoNivelActual == TipoNivel::Mamut) {
        ui->graphicsView->centerOn(cavermanSprite);
    }
}




void MainWindow::actualizarJuego()
{
    float dt = 0.016;

    nivel->actualizar(dt);

    caverman &jug = nivel->getJugador();
    float x = jug.getX();
    float paso = 2.5;

    if (moviendoIzquierda) x -= paso;
    if (moviendoDerecha)   x += paso;

    // primero aplicas el movimiento horizontal
    jug.setPos(x, jug.getY());

    // luego actualizas el salto (esto cambia solo la Y)
    jug.actualizarSalto(dt);

    // Limitar a la escena (como antes)
    QRectF limites = scene->sceneRect();
    if (jug.getX() < limites.left())  jug.setPos(limites.left(), jug.getY());
    if (jug.getX() > limites.right()) jug.setPos(limites.right(), jug.getY());
    if (jug.getY() < limites.top())   jug.setPos(jug.getX(), limites.top());
    if (jug.getY() > limites.bottom())jug.setPos(jug.getX(), limites.bottom());

    //nivel mamut
    if (tipoNivelActual == TipoNivel::Mamut && mamutSprite) {
        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());
        mamut &boss = nivelMamut->getMamut();

        mamutSprite->setPos(boss.getX(), boss.getY());

        // Actualizar dirección según velocidad
        if (boss.getVelX() < 0) {
            mamutSprite->setDireccion(sprite::Direccion::Izquierda);
        } else if (boss.getVelX() > 0) {
            mamutSprite->setDireccion(sprite::Direccion::Derecha);
        }
    }

    // Actualizar sprite
    if (cavermanSprite) {
        cavermanSprite->setPos(jug.getX(), jug.getY());
    }
    // 3) Sincronizar las bolas de fuego lógicas con los sprites gráficos
    sincronizarBolasConNivel();

    bool huboColision = evaluarColisionBolasConJugador();
    if (huboColision) {
        qDebug() << "El caverman recibió un golpe. Vida:" << nivel->getJugador().getVida();
    }
    // Verificar si toca cambiar de nivel ---
    if (nivel->estaCompletado()) {
        cambiarAlSiguienteNivel();
    }
    else if (nivel->estaFallido()) {
        // Aquí puedes reiniciar el mismo nivel, mostrar "Game Over", etc.
        // cargarNivel(tipoNivelActual);
    }
}
void MainWindow::cambiarAlSiguienteNivel()
{
    switch (tipoNivelActual) {
    case TipoNivel::Volcan:
        cargarNivel(TipoNivel::Mamut);
        break;
    case TipoNivel::Mamut:
        cargarNivel(TipoNivel::JefeSnow);
        break;
    case TipoNivel::JefeSnow:
        // Juego terminado: podrías parar el timer, mostrar mensaje, etc.
        // timerJuego->stop();
        break;
    }
}
//colisiones
bool MainWindow::evaluarColisionBolasConJugador()
{
    bool colision = false;

    for (int i = 0; i < bolasSprites.size(); i++) {

        // Detectar colisión gráfica inmediata
        if (cavermanSprite->collidesWithItem(bolasSprites[i]))
        {
            colision = true;

            // 1) Eliminar roca INSTANTÁNEAMENTE
            scene->removeItem(bolasSprites[i]);
            delete bolasSprites[i];
            bolasSprites.erase(bolasSprites.begin() + i);
            i--; // ajustar índice porque borramos

            // 2) Lógica del caverman (vida o efecto)
            caverman &jug = nivel->getJugador();
            jug.recibirDanio(10);   // si usas vida

            // 3) Sprite en estado de daño
            cavermanSprite->mostrarDanio();

            // como ya hubo colisión, podemos seguir
            continue;
        }
    }

    return colision;
}
void MainWindow::sincronizarBolasConNivel()
{
    // 1) Obtener proyectiles lógicos del nivel (genérico)
    const std::vector<proyectil*>& rocas = nivel->getProyectiles();

    // 2) Ajustar cantidad de sprites a la cantidad de rocas
    while (bolasSprites.size() > rocas.size()) {
        QGraphicsPixmapItem* item = bolasSprites.back();
        scene->removeItem(item);
        delete item;
        bolasSprites.pop_back();
    }

    QPixmap bolaPixmap(":/sprites/bola_fuego.png");
    while (bolasSprites.size() < rocas.size()) {
        QGraphicsPixmapItem* item = scene->addPixmap(bolaPixmap);
        item->setZValue(5);
        item->setScale(0.4);
        bolasSprites.push_back(item);
    }

    // 3) Actualizar posición
    for (int i = 0; i < rocas.size(); i++) {
        float x = rocas[i]->getX();
        float y = rocas[i]->getY();
        bolasSprites[i]->setPos(x, y);
    }
}


