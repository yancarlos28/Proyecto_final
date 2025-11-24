#include "mainwindow.h"
#include "nivelmamut.h"
#include "nivelvolcan.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>
#include <cmath>


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
    this->setFixedSize(ui->graphicsView->width() + 10,ui->graphicsView->height() + 20);

    // Sprite del jugador
    scene->addItem(cavermanSprite);
    cavermanSprite->setScale(2.2);
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

//Destructor UI
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
    if (tipoNivelActual == TipoNivel::Mamut &&
        event->key() == Qt::Key_X) {

        NivelMamut *nm = dynamic_cast<NivelMamut*>(nivel.get());
        if (nm) {
            nm->intentarAgarrarSoga();
        }
        return;
    }
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

    } else if (event->key() == Qt::Key_W) {
        jug.iniciarSalto();

        if (cavermanSprite) {
            cavermanSprite->setEstado(sprite::Estado::Saltar);
        }
        return;

    } else if (event->key() == Qt::Key_Space) {
        float dirX = 1.0;
        if (cavermanSprite &&
            cavermanSprite->getDireccion() == sprite::Direccion::Izquierda) {
            dirX = -1.0f;
        }

        NivelMamut *nivelMamut = dynamic_cast<NivelMamut*>(nivel.get());
        if (nivelMamut != nullptr) {
            nivelMamut->lanzarDesdeJugador(dirX);
        }

        return;

    }else {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (teclaMovimiento && cavermanSprite) {
        cavermanSprite->setEstado(sprite::Estado::Caminar);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (tipoNivelActual == TipoNivel::Mamut &&
        event->key() == Qt::Key_X) {

        NivelMamut *nm = dynamic_cast<NivelMamut*>(nivel.get());
        if (nm) {
            nm->soltarSoga();
        }
        return;
    }
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


//Actualizar juego y tiempo
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
    if (tipoNivelActual == TipoNivel::Mamut && mamutSprite &&sogaItem) {
        ui->graphicsView->centerOn(cavermanSprite);
        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());
        mamut &boss = nivelMamut->getMamut();

        mamutSprite->setPos(boss.getX(), boss.getY());
        QPointF ancla(nivelMamut->getSogaXAncla(), nivelMamut->getSogaYAncla());
        QPointF extremo(nivelMamut->getSogaXExtremo(), nivelMamut->getSogaYExtremo());
        sogaItem->setLine(QLineF(ancla, extremo));

        sogaItem->setVisible(true);
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
    //--Para mamut sincronizar bolas
    sincronizarLanzasConNivel();

    bool huboColision = evaluarColisionBolasConJugador();
    if (huboColision) {
        qDebug() << "El caverman recibió un golpe. Vida:" << nivel->getJugador().getVida();
    }
    // Verificar si toca cambiar de nivel ---
    if (nivel->estaCompletado()) {
        cambiarAlSiguienteNivel();
    }
    bool huboImpactoLanzas = evaluarColisionLanzasConMamut();
    if (huboImpactoLanzas) {
        qDebug() << "[Mamut] Recibió impacto de lanza. Vida:"
                 << dynamic_cast<NivelMamut*>(nivel.get())->getMamut().getVida();
    }

    else if (nivel->estaFallido()) {
        // Aquí puedes reiniciar el mismo nivel, mostrar "Game Over", etc.
        // cargarNivel(tipoNivelActual);
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

//Opciones de nivel
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
        QImage imgEscalada = img.scaled(1536, 1024,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // Volcán: nivel por tiempo (ejemplo 40 s)
        duracionNivel =40;
        break;
    }

    case TipoNivel::Mamut:
    {
        nivel = std::make_unique<NivelMamut>();

        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());

        float anchoMundo = nivelMamut->getAnchoMundo();
        scene->setSceneRect(0, 0, anchoMundo, 1024);

        QImage img(":/escenas/bosque.jpg");
        QImage imgEscalada = img.scaled(1536,1024 ,
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
        // Crear soga gráfica
        if (sogaItem == nullptr) {
            sogaItem = new QGraphicsLineItem();
            QPen pen(Qt::darkGray, 4);
            sogaItem->setPen(pen);
            sogaItem->setZValue(8);
            scene->addItem(sogaItem);
        }
        sogaItem->setVisible(true);

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
    for (int i = 0; i < lanzasSprites.size(); ++i) {
        scene->removeItem(lanzasSprites[i]);
        delete lanzasSprites[i];
    }
    lanzasSprites.clear();
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
            jug.recibirDanio(20);   // si usas

            // 3) Sprite en estado de daño
            cavermanSprite->mostrarDanio();

            // como ya hubo colisión, podemos seguir
            continue;
        }
    }

    return colision;
}
bool MainWindow::evaluarColisionMamutConJugador()
{
    bool colision = false;
        // Detectar colisión gráfica inmediata
        if (cavermanSprite->collidesWithItem(mamutSprite))
        {
            colision = true;
            // Lógica del caverman (vida o efecto)
            caverman &jug = nivel->getJugador();
            jug.recibirDanio(20);   // si usas

            //Sprite en estado de daño
            cavermanSprite->mostrarDanio();

            // como ya hubo colisión, podemos seguir
        }


    return colision;
}
bool MainWindow::evaluarColisionLanzasConMamut()
{
    // Solo tiene sentido en el nivel Mamut y si existe el sprite del mamut
    if (tipoNivelActual != TipoNivel::Mamut || mamutSprite == nullptr) {
        return false;
    }

    NivelMamut *nivelMamut = static_cast<NivelMamut*>(nivel.get());
    if (nivelMamut == nullptr) {
        return false;
    }

    const std::vector<proyectil*> &lanzasLogicas = nivelMamut->getProyectilesJugador();
    bool huboImpacto = false;

    int i = 0;
    while (i < lanzasSprites.size() && i < lanzasLogicas.size()) {

        QGraphicsPixmapItem *lanzaItem = lanzasSprites[i];

        // ¿Colisiona el sprite de la lanza con el sprite del mamut?
        if (lanzaItem->collidesWithItem(mamutSprite)) {
            huboImpacto = true;

            proyectil *p = lanzasLogicas[i];
            int danio = p->getDanio();

            mamut &boss = nivelMamut->getMamut();
            boss.recibirDanio(danio);

            // 🔹 Cambiar sprite del mamut según su vida
            if (boss.getVida() <= 0) {
                // Mamut muerto → sprite final
                mamutSprite->matarMamut();

                // Opcional: aquí puedes parar su movimiento lógico
                // boss.setVel(0, 0);
                // yad marcar el nivel como completado en NivelMamut
            } else {
                // Mamut herido → sprite de daño un momento
                mamutSprite->mostrarDanioMamut();
            }

            // 3) Borrar sprite de la lanza
            scene->removeItem(lanzaItem);
            delete lanzaItem;
            lanzasSprites.erase(lanzasSprites.begin() + i);

            // 4) Borrar la lanza lógica en el mismo índice
            nivelMamut->eliminarLanzaEnIndice(i);

            continue;
        }

        // Si no hubo colisión, siguiente lanza
        i++;
    }

    return huboImpacto;
}

//sincronizacion acciones
void MainWindow::sincronizarBolasConNivel()
{
    // 1) Obtener proyectiles lógicos del nivel (genérico)
    const std::vector<proyectil*>& rocas = nivel->getProyectilesEnemigos();

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
void MainWindow::sincronizarLanzasConNivel()
{
    NivelMamut *nivelmamut = dynamic_cast<NivelMamut*>(nivel.get());
    if (nivelmamut == nullptr) return;

    const vector<proyectil*> &lanzas = nivelmamut->getProyectilesJugador();

    // 1) Ajustar cantidad de sprites
    while (lanzasSprites.size() > lanzas.size()) {
        QGraphicsPixmapItem* item = lanzasSprites.back();
        scene->removeItem(item);
        delete item;
        lanzasSprites.pop_back();
    }

    QPixmap lanzaPixmap(":/sprites/lanza.png");
    while (lanzasSprites.size() < lanzas.size()) {
        QGraphicsPixmapItem* item = scene->addPixmap(lanzaPixmap);
        item->setZValue(6);
        item->setScale(1.5);

        // 🔹 ORIGEN DE ROTACIÓN EN EL CENTRO DEL PIXMAP
        int w = lanzaPixmap.width();
        int h = lanzaPixmap.height();
        item->setTransformOriginPoint(w / 2.0, h / 2.0);

        lanzasSprites.push_back(item);
    }


    // 2) Actualizar posiciones
    int i;
    for (i = 0; i < lanzas.size(); ++i) {
        proyectil* p = lanzas[i];
        QGraphicsPixmapItem* item = lanzasSprites[i];

        // Posicion
        item->setPos(p->getX(), p->getY());

        // Velocidad actual del proyectil
        float vx = p->getVelX();
        float vy = p->getVelY();

        // Evitar problemas cuando vx == 0 (lanza totalmente vertical)
        if (vx == 0.0f && vy == 0.0f) {
            continue;
        }

        // Ángulo de la velocidad (tangente a la trayectoria)
        double angRad = atan2(vy, vx);          // radianes
        double angDeg = angRad * 180.0 / 3.14159265; // grados

        // Aplicar rotación: el sprite apunta en la dirección del movimiento
        item->setRotation(angDeg);
    }

}
