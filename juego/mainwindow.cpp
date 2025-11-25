#include "mainwindow.h"
#include "nivelmamut.h"
#include "nivelsnowman.h"
#include "nivelvolcan.h"
#include "nivelsnowman.h"

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


    // Barra de vida jugador
    barraVidaSheet.load(":/recursos_juego/corazones.PNG");

    if (barraVidaSheet.isNull()) {
        qDebug() << "ERROR: no se pudo cargar corazones.png";
    } else {
        // Ancho de una fila completa
        vidaFrameAncho = barraVidaSheet.width();
        vidaFrameAlto  = barraVidaSheet.height() / 6;
        // Primer frame = fila 0 (vida completa)
        QPixmap frame = barraVidaSheet.copy(0, 0,vidaFrameAncho,vidaFrameAlto);
        barraVidaItem = scene->addPixmap(frame);
        barraVidaItem->setZValue(100);
        barraVidaItem->setScale(0.4);
        barraVidaItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }

    // Cargar el primer nivel
    cargarNivel(TipoNivel::JefeSnow);

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
    if (event->isAutoRepeat()&& event->key() != Qt::Key_X) {
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
    //Izquierda
    if (event->key() == Qt::Key_A) {
        moviendoIzquierda = true;
        teclaMovimiento = true;
        if (cavermanSprite)
            cavermanSprite->setDireccion(sprite::Direccion::Izquierda);

    }
    //Derecha
    else if (event->key() == Qt::Key_D) {
        moviendoDerecha = true;
        teclaMovimiento = true;
        if (cavermanSprite)
            cavermanSprite->setDireccion(sprite::Direccion::Derecha);

    }
    //Saltar
    else if (event->key() == Qt::Key_W) {
        jug.iniciarSalto();

        if (cavermanSprite) {
            cavermanSprite->setEstado(sprite::Estado::Saltar);
        }
        return;

    }
    //Lanzar lanzas
    else if (event->key() == Qt::Key_Space) {
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

    }
    else {
        QMainWindow::keyPressEvent(event);
        return;
    }

    //Caminar
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

    // Movimiento en x
    jug.setPos(x, jug.getY());

    // Actualizar salto
    jug.actualizarSalto(dt);

    // Limitar a la escena
    QRectF limites = scene->sceneRect();
    if (jug.getX() < limites.left())  jug.setPos(limites.left(), jug.getY());
    if (jug.getX() > limites.right()) jug.setPos(limites.right(), jug.getY());
    if (jug.getY() < limites.top())   jug.setPos(jug.getX(), limites.top());
    if (jug.getY() > limites.bottom())jug.setPos(jug.getX(), limites.bottom());

    //Nivel mamut
    if (tipoNivelActual == TipoNivel::Mamut && mamutSprite &&sogaItem) {
        //Centrar al jugador
        ui->graphicsView->centerOn(cavermanSprite);

        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());
        mamut &boss = nivelMamut->getMamut();

        //Posicion mamut
        mamutSprite->setPos(boss.getX(), boss.getY());

        //Para la soga del nivel, obtener posiciones
        QPointF ancla(nivelMamut->getSogaXAncla(), nivelMamut->getSogaYAncla());
        QPointF extremo(nivelMamut->getSogaXExtremo(), nivelMamut->getSogaYExtremo());
        sogaItem->setLine(QLineF(ancla, extremo));
        sogaItem->setVisible(true);

        // Actualizar dirección según velocidad del mamut
        if (boss.getVelX() < 0) {
            mamutSprite->setDireccion(sprite::Direccion::Izquierda);
        }
        else if (boss.getVelX() > 0) {
            mamutSprite->setDireccion(sprite::Direccion::Derecha);
        }
        //Lanzas con mamut
        sincronizarLanzasConNivel();
        bool huboImpactoLanzas = evaluarColisionLanzasConMamut();
        if (huboImpactoLanzas) {
            qDebug() << "[Mamut] Recibió impacto de lanza. Vida:"
                     << static_cast<NivelMamut*>(nivel.get())->getMamut().getVida();
        }
        //Mamut con jugador
        bool huboColisionMamut = evaluarColisionMamutConJugador();
        if (huboColisionMamut) {
            qDebug() << "[Jugador] Golpeado por mamut. Vida:"
                     << nivel->getJugador().getVida();
        }

    }

    if (tipoNivelActual == TipoNivel::Volcan) {
        // Sincronizar las bolas de fuego lógicas con los sprites gráficos
        sincronizarBolasConNivel();
        bool huboColision = evaluarColisionBolasConJugador();
        if (huboColision) {
            qDebug() << "El caverman recibió un golpe. Vida:" << nivel->getJugador().getVida();
        }
    }
    if (tipoNivelActual == TipoNivel::JefeSnow) {

        // 1) Mover sprite del Snowman según la lógica
        NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
        if (ns && snowmanSprite) {
            Snowman &boss = ns->getSnowman();
            snowmanSprite->setPos(boss.getX(), boss.getY());
        }

        // 2) Sincronizar bolas de nieve (parabólicas + oscilantes)
        sincronizarBolasNieveConNivel();

        // 3) Colisión bolas de nieve con jugador
        bool golpeNieve = evaluarColisionBolasNieveConJugador();
        if (golpeNieve) {
            actualizarBarraVida(jug.getVida());
        }
    }

    // Actualizar sprite
    if (cavermanSprite) {
        cavermanSprite->setPos(jug.getX(), jug.getY());
    }


    // Posicionar barra de vida en la pantalla ---
    if (barraVidaItem) {
        QPointF esquina = ui->graphicsView->mapToScene(0, 0);
        float margenX = 40.0;
        float margenY = 40.0;
        barraVidaItem->setPos(esquina.x() + margenX,esquina.y() + margenY);
    }


    // Verificar CAMBIO DE NIVEL ---
    if (nivel->estaCompletado()) {
        cambiarAlSiguienteNivel();
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
void MainWindow::actualizarBarraVida(int vida)
{
    if (!barraVidaItem) return;
    if (barraVidaSheet.isNull()) return;

    if (vida < 0) vida = 0;
    if (vida > 100) vida = 100;

    int perdida = 100 - vida;           // 0,20,40,60,80,100
    int fila = perdida / 20;            // 0,1,2,3,4,5

    if (fila < 0) fila = 0;
    if (fila > 5) fila = 5;

    int srcX = 0;
    int srcY = fila * vidaFrameAlto;    // 0,100,200,300,400,500

    QPixmap frame = barraVidaSheet.copy(srcX, srcY,vidaFrameAncho,vidaFrameAlto);

    // Borrar la anterior y poner nueva
    barraVidaItem->setPixmap(frame);
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
    //scene->clear();
    // Crear el nuevo nivel lógico
    switch (tipo) {
    case TipoNivel::Volcan:
    {
        nivel = std::make_unique<NivelVolcan>();
        scene->setSceneRect(0, 0, 1536, 1024);

        QImage img("C:/Users/Jean/OneDrive/Documentos/INFORMATICA_2/Laboratorio/game/escenas/volcan.png");
        QImage imgEscalada = img.scaled(1536, 1024,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // Duración nivel volcán
        duracionNivel =40;
        break;
    }

    case TipoNivel::Mamut:
    {
        nivel = std::make_unique<NivelMamut>();
        //Central la vista
        ui->graphicsView->centerOn(cavermanSprite);

        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());

        float anchoMundo = nivelMamut->getAnchoMundo();
        scene->setSceneRect(0, 0, anchoMundo, 1024);

        QImage img(":/escenas/bosque.jpg");
        QImage imgEscalada = img.scaled(1536,1024 ,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // Sprite del Mamut
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
        // 1) Crear nivel lógico
        nivel = std::make_unique<NivelSnowman>();

        // 2) Tamaño de la escena para este nivel
        //    Si tu imagen de fondo es 1920x1200 úsalo tal cual:
        scene->setSceneRect(0, 0, 1920, 1200);

        // 3) Fondo de nieve (desde recursos o desde archivo)
        QImage img(":/escenas/nieve.jpg");  // o la ruta que uses
        QImage imgEscalada = img.scaled(1920, 1200,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // 4) Snowman lógico (enemigo)
        NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
        if (ns) {
            // Crear sprite del Snowman sólo una vez
            if (!snowmanSprite) {
                snowmanSprite = new sprite(this, sprite::Tipo::Snowman);
                scene->addItem(snowmanSprite);
                snowmanSprite->setZValue(5);
                snowmanSprite->setScale(4.0);    // ajusta tamaño si se ve muy grande/pequeño
            }

            // Posición inicial según el modelo lógico
            Snowman &boss = ns->getSnowman();
            snowmanSprite->setPos(boss.getX(), boss.getY());
        }

        // 5) Este nivel no tiene temporizador por ahora
        duracionNivel = 0;
        break;
    }

    }

    tipoNivelActual = tipo;

    //Reiniciar temporizador visual del nivel
    segundosRestantesNivel = duracionNivel;
    if (duracionNivel > 0) {
        ui->lblTemporizadorNivel->setVisible(true);
        ui->lblTemporizadorNivel->setText(
            QString("Restante: %1 s").arg(segundosRestantesNivel));
    }
    else {
        ui->lblTemporizadorNivel->setVisible(false);
    }

    //Reposicionar el sprite según la posición lógica del jugador
    caverman &j = nivel->getJugador();
    cavermanSprite->setPos(j.getX(), j.getY());

    // Inicializar barra de vida con la vida actual del jugador
    actualizarBarraVida(j.getVida());

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
    if (!cavermanSprite) return false;

    NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
    if (!ns) return false;

    bool huboColision = false;

    for (size_t i = 0; i < bolasNieveSprites.size(); ++i) {

        if (cavermanSprite->collidesWithItem(bolasNieveSprites[i])) {

            huboColision = true;

            // 1) Eliminar bola lógica en el nivel
            ns->eliminarBolaPorIndice(i);

            // 2) Eliminar sprite gráfico correspondiente
            scene->removeItem(bolasNieveSprites[i]);
            delete bolasNieveSprites[i];
            bolasNieveSprites.erase(bolasNieveSprites.begin() + i);

            // 3) Aplicar daño al jugador (por ejemplo 10)
            caverman &jug = nivel->getJugador();
            jug.recibirDanio(10);
            cavermanSprite->mostrarDanio();

            // solo un golpe por frame
            break;
        }
    }

    return huboColision;
}
bool MainWindow::evaluarColisionMamutConJugador()
{
    if (tipoNivelActual != TipoNivel::Mamut || mamutSprite == nullptr) {
        return false;
    }

    bool colision = false;

    // Detectar colisión gráfica
    if (cavermanSprite->collidesWithItem(mamutSprite))
    {
        colision = true;

        // 1) Lógica del caverman (vida)
        caverman &jug = nivel->getJugador();
        jug.recibirDanio(1);

        // 2) Mostrar daño en el sprite
        cavermanSprite->mostrarDanio();

        // 3) Actualizar barra de corazones
        actualizarBarraVida(jug.getVida());
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
bool MainWindow::evaluarColisionBolasNieveConJugador()
{
    if (!cavermanSprite) return false;

    bool huboColision = false;

    for (size_t i = 0; i < bolasNieveSprites.size(); ++i) {
        if (cavermanSprite->collidesWithItem(bolasNieveSprites[i])) {

            // 1) eliminar proyectil lógico
            NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
            //if (ns) {
              //  ns->eliminarBolaPorIndice(i); // implementa esto en el nivel
            //}

            // 2) eliminar sprite gráfico
            scene->removeItem(bolasNieveSprites[i]);
            delete bolasNieveSprites[i];
            bolasNieveSprites.erase(bolasNieveSprites.begin() + i);

            // 3) aplicar daño al jugador
            caverman &jug = nivel->getJugador();
            jug.recibirDanio(10);          // o el valor que quieras
            cavermanSprite->mostrarDanio();

            huboColision = true;
            break; // solo 1 golpe por frame
        }
    }

    return huboColision;
}

//sincronizacion acciones
void MainWindow::sincronizarBolasConNivel()
{
    // 1) Obtener proyectiles lógicos del nivel
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
void MainWindow::sincronizarBolasNieveConNivel()
{
    NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
    if (!ns) return;

    const auto &parabolicas = ns->getBolasParabolicas(); // vector<proyectil*>
    const auto &oscilantes = ns->getBolasOscilantes();

    // Número total de bolas (parabólicas + oscilantes)
    size_t total = parabolicas.size() + oscilantes.size();

    // 1) Si sobran sprites, los borramos
    while (bolasNieveSprites.size() > total) {
        QGraphicsPixmapItem *item = bolasNieveSprites.back();
        scene->removeItem(item);
        delete item;
        bolasNieveSprites.pop_back();
    }

    // 2) Si faltan sprites, creamos nuevos
    QPixmap bolaNievePixmap(":/sprites/bola_nieve.png");
    while (bolasNieveSprites.size() < total) {
        QGraphicsPixmapItem *item = scene->addPixmap(bolaNievePixmap);
        item->setZValue(6);      // por encima del fondo
        item->setScale(1.5);     // ajusta tamaño si hace falta
        bolasNieveSprites.push_back(item);
    }

    // 3) Actualizar posiciones: primero las parabólicas, luego las oscilantes
    size_t k = 0;

    for (size_t i = 0; i < parabolicas.size(); ++i) {
        proyectil *p = parabolicas[i];
        bolasNieveSprites[k]->setPos(p->getX(), p->getY());
        ++k;
    }

    for (size_t i = 0; i < oscilantes.size(); ++i) {
        const BolaNieveOscilante &b = oscilantes[i];
        bolasNieveSprites[k]->setPos(b.x, b.y);
        ++k;
    }
}
