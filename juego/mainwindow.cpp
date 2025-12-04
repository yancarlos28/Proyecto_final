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

    // Config de la vista
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, 1536, 1024);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setFixedSize(1536, 1024);
    this->setFixedSize(ui->graphicsView->width() + 10,ui->graphicsView->height() + 20);

    // Arrancar en el menú
    irAlMenu();

    //Lanzas mamut
    QPixmap hudFlecha(":/sprites/lanzas_3.png");
    QPixmap flechaEscalada = hudFlecha.scaled(ui->lblIconoFlecha->width()-60,ui->lblIconoFlecha->height()-20,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
    ui->lblIconoFlecha->setPixmap(flechaEscalada);
    ui->lblIconoFlecha->setScaledContents(true); // opcional, por si el tamaño del label cambia
    ui->lblMunicion->setText("0");

    // --- Inicializar sonidos ---
    playerMamut = new QMediaPlayer(this);
    audioMamut  = new QAudioOutput(this);
    playerMamut->setAudioOutput(audioMamut);
    audioMamut->setVolume(0.5);
    playerMamut->setSource(QUrl("qrc:/sonidos/jungle.mp3"));
    playerMamut->stop();

    playerVolcan = new QMediaPlayer(this);
    audioVolcan  = new QAudioOutput(this);
    playerVolcan->setAudioOutput(audioVolcan);
    audioVolcan->setVolume(0.5);
    playerVolcan->setSource(QUrl("qrc:/sonidos/volcan.mp3"));
    playerVolcan->stop();

    // Barra de vida del enemigo
    ui->pbVidaEnemigo->setMinimum(0);
    ui->pbVidaEnemigo->setMaximum(300);   // por ejemplo, vida máxima del mamut/snowman
    ui->pbVidaEnemigo->setFixedSize(300, 250);   // ancho 250px, alto 25px
    ui->pbVidaEnemigo->setValue(0);
    ui->pbVidaEnemigo->move(1050, 80); // X = 1200, Y = 20  (ejemplo)
    ui->pbVidaEnemigo->setTextVisible(false);


    ui->pbVidaEnemigo->hide();            // ocúltala al inicio



    // Esconde al inicio
    ui->lblMunicion->hide();
    ui->lblMunicion->hide();

}

//Destructor UI
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::irAlMenu()
{
    ui->pbVidaEnemigo->hide();
    //Asegurar el stop de la música
    if (playerMamut)  playerMamut->stop();
    if (playerVolcan) playerVolcan->stop();

    estadoJuego = EstadoJuego::EnMenu;
    ui->lblIconoFlecha->hide();
    ui->lblMunicion->hide();

    // Detener timers
    if (timerJuego) {
        timerJuego->stop();
    }
    if (timerTiempo) {
        timerTiempo->stop();
    }

    // Quitar sprite del jugador y barra de vida de la escena (pero sin destruirlos)
    if (cavermanSprite && cavermanSprite->scene()) {
        scene->removeItem(cavermanSprite);
    }
    if (barraVidaItem && barraVidaItem->scene()) {
        scene->removeItem(barraVidaItem);
    }

    // Limpiar sprites del nivel (mamut, bolas, lanzas, etc.)
    limpiarSpritesNivel();

    // Poner fondo y aspecto de menú
    mostrarMenu();

    // Mostrar botones del menú (debes tener 3 en el .ui)
    ui->pushButtonVolcan->show();
    ui->pushButtonMamut->show();
    ui->pushButtonSnow->show();
}

void MainWindow::mostrarMenu()
{
    scene->clear();
    scene->setSceneRect(0, 0, 1536, 1024);

    // Fondo del menú
    QImage img(":/escenas/menu.jpg");
    QImage imgEscalada = img.scaled(1536, 1024,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

    // Ocultar labels de juego mientras estamos en el menú
    if (ui->lblTiempoJuego)       ui->lblTiempoJuego->setVisible(false);
    if (ui->lblTemporizadorNivel) ui->lblTemporizadorNivel->setVisible(false);

}

void MainWindow::iniciarNivel(TipoNivel tipo)
{
    estadoJuego = EstadoJuego::EnNivel;

    // Ocultar botones del menú
    ui->pushButtonVolcan->hide();
    ui->pushButtonMamut->hide();
    ui->pushButtonSnow->hide();

    // Volver visibles los labels del juego
    if (ui->lblTiempoJuego)       ui->lblTiempoJuego->setVisible(true);
    if (ui->lblTemporizadorNivel) ui->lblTemporizadorNivel->setVisible(true);

    // Añadir sprite del jugador si no está ya en la escena
    if (cavermanSprite && !cavermanSprite->scene()) {
        scene->addItem(cavermanSprite);
        cavermanSprite->setScale(2.2);
        cavermanSprite->setZValue(10);
    }

    // Barra de vida: si aún no se ha creado, la creamos
    if (!barraVidaItem) {
        barraVidaSheet.load(":/recursos_juego/corazones.PNG");
        if (barraVidaSheet.isNull()) {
            qDebug() << "ERROR: no se pudo cargar corazones.png";
        } else {
            vidaFrameAncho = barraVidaSheet.width();
            vidaFrameAlto  = barraVidaSheet.height() / 6;
            QPixmap frame = barraVidaSheet.copy(0, 0,vidaFrameAncho,vidaFrameAlto);
            barraVidaItem = scene->addPixmap(frame);
            barraVidaItem->setZValue(100);
            barraVidaItem->setScale(0.4);
            barraVidaItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        }
    } else if (!barraVidaItem->scene()) {
        // si ya existe pero no está en la escena, volverlo a agregar
        scene->addItem(barraVidaItem);
    }

    // Cargar la lógica y el fondo del nivel
    cargarNivel(tipo);

    // Crear/arrancar timerJuego
    if (!timerJuego) {
        timerJuego = new QTimer(this);
        connect(timerJuego, &QTimer::timeout,this, &MainWindow::actualizarJuego);
    }
    timerJuego->start(16);

    // Crear/arrancar timerTiempo
    if (!timerTiempo) {
        timerTiempo = new QTimer(this);
        connect(timerTiempo, &QTimer::timeout,this, &MainWindow::actualizarTiempo);
    }
    timerTiempo->start(1000);

    // Reiniciar contadores si quieres
    segundosJuego = 0;
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
        break;
    }
}
void MainWindow::cargarNivel(TipoNivel tipo)
{
    limpiarSpritesNivel();

    // Crear el nuevo nivel lógico
    switch (tipo) {
    case TipoNivel::Volcan:
    {
        if (playerVolcan) {
            playerVolcan->stop();
            playerVolcan->play();
        }
        nivel = std::make_unique<NivelVolcan>();
        scene->setSceneRect(0, 0, 1536, 1024);


        QImage img("C:/Users/Jean/OneDrive/Documentos/INFORMATICA_2/Laboratorio/game/escenas/volcan.png");
        QImage imgEscalada = img.scaled(1536, 1024,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        scene->setBackgroundBrush(QPixmap::fromImage(imgEscalada));

        // Duración nivel volcán
        duracionNivel =40;
        // 👉 Posición inicial del jugador en volcán
        caverman &jug = nivel->getJugador();
        volcanInicioX = jug.getX();
        volcanInicioY = jug.getY();

        // 👉 Crear sprite de la meta
        QPixmap metaPix(":/recursos_juego/meta_volcan.png");   // cambia a tu ruta real
        metaVolcanItem = scene->addPixmap(metaPix);
        metaVolcanItem->setZValue(8);
        metaVolcanItem->setScale(0.1);          // la haces más pequeña si quieres

        // Pon la meta lejos para que tenga que ir hasta allá
        // (ajusta estas coordenadas a donde quieras que esté la meta)
        metaVolcanItem->setPos(1410, 560);

        // Estado inicial
        metaVolcanTomada = false;
        metaVolcanCompletada = false;
        break;
    }

    case TipoNivel::Mamut:
    {
        if (playerMamut) {
            playerMamut->stop();
            playerMamut->play();
        }


        ui->lblIconoFlecha->show();
        ui->lblMunicion->show();

        nivel = std::make_unique<NivelMamut>();
        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());

        //Central la vista
        ui->graphicsView->centerOn(cavermanSprite);

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
        ui->pbVidaEnemigo->setMinimum(0);
        ui->pbVidaEnemigo->setMaximum(200);           // VIDA MÁXIMA FIJA DEL MAMUT
        ui->pbVidaEnemigo->setValue(boss.getVida());  // empieza en 200
        ui->pbVidaEnemigo->show();

        // Crear soga gráfica
        if (sogaItem == nullptr) {
            sogaItem = new QGraphicsLineItem();
            QPen pen(Qt::darkGreen, 4);
            sogaItem->setPen(pen);
            sogaItem->setZValue(8);
            scene->addItem(sogaItem);
        }
        sogaItem->setVisible(true);
        duracionNivel = 0;
        break;

    }

    case TipoNivel::JefeSnow:
    {
        // 1) Crear nivel lógico
        nivel = std::make_unique<NivelSnowman>();
        termometroSheet.load(":/recursos_juego/termometro_escala.png");

        QPixmap frameTerm = termometroSheet.copy(
            0,              // x = 0 (primer termómetro)
            0,              // y = 0
            termFrameAncho, // 100
            termFrameAlto   // 293
            );
        // Lo agregamos a la escena
        termometroItem = scene->addPixmap(frameTerm);
        // Ubícalo donde quieras (por ejemplo, arriba a la izquierda)
        termometroItem->setPos(1400, 40);
        termometroItem->setZValue(1000); // bien encima de todo
        termometroItem->setScale(0.3);
        // 2) Tamaño de la escena para este nivel
        scene->setSceneRect(0, 0, 1920, 1200);


        // 3) Fondo de nieve
        QImage img(":/escenas/hielo.jpeg");
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
                snowmanSprite->setScale(4.0);
            }
            // Posición inicial según el modelo lógico
            Snowman &boss = ns->getSnowman();
            snowmanSprite->setPos(boss.getX(), boss.getY());
            ui->pbVidaEnemigo->setMinimum(0);
            ui->pbVidaEnemigo->setMaximum(300);           // VIDA MÁXIMA FIJA DEL SNOWMAN
            ui->pbVidaEnemigo->setValue(boss.getVida());  // empieza en 300
            ui->pbVidaEnemigo->show();
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
        ui->lblTemporizadorNivel->setText(QString("Restante: %1 s").arg(segundosRestantesNivel));
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
void MainWindow::mostrarPantallaResultado(bool gano)
{
    // Si ya se está mostrando, no hacer nada
    if (mostrandoResultado) return;
    mostrandoResultado = true;

    // Detener cualquier música de nivel
    if (playerMamut)  playerMamut->stop();
    if (playerVolcan) playerVolcan->stop();

    // Detener timers de juego para "congelar" todo
    if (timerJuego)  timerJuego->stop();
    if (timerTiempo) timerTiempo->stop();

    // Elegir imagen según ganó / perdió
    QPixmap img(gano ? ":/recursos_juego/you_win.jpg"
                     : ":/recursos_juego/game_over.jpg");

    QRectF r = scene->sceneRect();
    QPixmap imgEscalada;

    // --- ESCALADO ---
    if (tipoNivelActual == TipoNivel::Mamut) {
        // MAMUT: lo dejamos como tú lo tenías (tamaño original del JPG)
        imgEscalada = img.scaled(
            img.width(),
            img.height(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
    } else {
        // OTROS NIVELES (Volcán, Snowman): imagen más pequeña, NO pantalla completa
        const int maxAncho = 700;  // ajusta si la quieres más grande/pequeña
        const int maxAlto  = 500;

        imgEscalada = img.scaled(
            maxAncho,
            maxAlto,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
    }

    // Crear / actualizar el item
    if (!resultadoItem) {
        resultadoItem = scene->addPixmap(imgEscalada);
    } else {
        resultadoItem->setPixmap(imgEscalada);
        if (!resultadoItem->scene()) {
            scene->addItem(resultadoItem);
        }
    }

    // --- POSICIÓN ---
    if (tipoNivelActual == TipoNivel::Mamut) {
        // Tu posición especial para mamut (la dejo igual)
        float x = r.center().x() - imgEscalada.width()  - 260;
        float y = r.center().y() - imgEscalada.height()+150;
        resultadoItem->setPos(x, y);
    }
    else if(tipoNivelActual == TipoNivel::JefeSnow)
    {
        float x = (r.center().x() - imgEscalada.width()  / 2.0)-200;
        float y = (r.center().y() - imgEscalada.height() / 2.0)-100;
        resultadoItem->setPos(x, y);
    }
    else {
        float x = (r.center().x() - imgEscalada.width()  / 2.0);
        float y = (r.center().y() - imgEscalada.height() / 2.0);
        resultadoItem->setPos(x, y);
    }

    resultadoItem->setZValue(9999);

    // Después de 1 segundo, quitar la imagen y volver al menú
    QTimer::singleShot(1000, this, [this]() {
        if (resultadoItem && resultadoItem->scene()) {
            scene->removeItem(resultadoItem);
        }
        mostrandoResultado = false;
        irAlMenu();
    });
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
    //Lanzas sprites
    for (int i = 0; i < lanzasSprites.size(); ++i)
    {
        scene->removeItem(lanzasSprites[i]);
        delete lanzasSprites[i];
    }
    lanzasSprites.clear();
    //Corazones Item
    for (QGraphicsPixmapItem *item : corazonesSprites) {
        scene->removeItem(item);
        delete item;
    }
    corazonesSprites.clear();
    // Antorcha Item
    if (antorchaItem) {
        scene->removeItem(antorchaItem);
        delete antorchaItem;
        antorchaItem = nullptr;
    }
    // Bolitas de fuego
    for (QGraphicsPixmapItem *item : bolitasFuegoSprites) {
        scene->removeItem(item);
        delete item;
    }
    bolitasFuegoSprites.clear();
    // Meta del volcán
    if (metaVolcanItem) {
        scene->removeItem(metaVolcanItem);
        delete metaVolcanItem;
        metaVolcanItem = nullptr;
    }
    metaVolcanTomada = false;
    metaVolcanCompletada = false;

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
    //Lanzar arma
    else if (event->key() == Qt::Key_Space) {

        if (tipoNivelActual == TipoNivel::Mamut) {
            float dirX = 1.0;
            if (cavermanSprite &&
                cavermanSprite->getDireccion() == sprite::Direccion::Izquierda) {
                dirX = -1.0f;
            }

            NivelMamut *nivelMamut = dynamic_cast<NivelMamut*>(nivel.get());
            if (nivelMamut != nullptr) {
                nivelMamut->lanzarDesdeJugador(dirX);
            }
        }
        else if (tipoNivelActual == TipoNivel::JefeSnow) {
            NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
            if (ns) {
                ns->lanzarAntorcha();
            }
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
    if (estadoJuego == EstadoJuego::EnMenu || !nivel) {
        QMainWindow::keyPressEvent(event);
        return;
    }
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
    if (!nivel) return;
    // Si estoy mostrando la pantalla de resultado, no actualizo nada más
    if (mostrandoResultado) return;
    float dt = 0.016;

    nivel->actualizar(dt);

    caverman &jug = nivel->getJugador();

    float x = jug.getX();
    float paso = 3;

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

    // Nivel mamut
    if (tipoNivelActual == TipoNivel::Mamut && mamutSprite && sogaItem) {
        ui->graphicsView->centerOn(cavermanSprite);
        NivelMamut* nivelMamut = static_cast<NivelMamut*>(nivel.get());

        mamut &boss = nivelMamut->getMamut();
        mamutSprite->setPos(boss.getX(), boss.getY());
        // Dirección del mamut según velocidad
        if (boss.getVelX() < 0) {
            mamutSprite->setDireccion(sprite::Direccion::Izquierda);
        } else if (boss.getVelX() > 0) {
            mamutSprite->setDireccion(sprite::Direccion::Derecha);
        }
        // Configurar soga en el nivel
        QPointF ancla(nivelMamut->getSogaXAncla(), nivelMamut->getSogaYAncla());
        QPointF extremo(nivelMamut->getSogaXExtremo(), nivelMamut->getSogaYExtremo());
        sogaItem->setLine(QLineF(ancla, extremo));
        sogaItem->setVisible(true);

        // Lanzas del jugador contra el mamut
        sincronizarLanzasConNivel();
        evaluarColisionLanzasConMamut();

        // Mamut contra jugador
        evaluarColisionMamutConJugador();

        //Flechas que caen
        sincronizarBolasConNivel();
        evaluarColisionFlechasConJugador();

        //Actualizar lanzas
        ui->lblMunicion->setText(QString::number(nivelMamut->getMunicionJugador()));

        caverman &jug = nivel->getJugador();
        if (jug.getVida() <= 0) {
            mostrarPantallaResultado(false);
            return;
        }
    }


    if (tipoNivelActual == TipoNivel::Volcan) {
        // Sincronizar las bolas de fuego lógicas con los sprites gráficos
        sincronizarBolasConNivel();
        bool huboColision = evaluarColisionBolasConJugador();
        if (huboColision) {
            qDebug() << "El caverman recibió un golpe. Vida:" << nivel->getJugador().getVida();
            actualizarBarraVida(jug.getVida());
        }
        if(nivel->getJugador().getVida()==0){
            qDebug() << "El caverman murió";
            mostrarPantallaResultado(false);
            playerVolcan->stop();
        }
        // Sincronizar corazones que caen
        sincronizarCorazonesVolcan();
        bool tomoCorazon = evaluarColisionCorazonesConJugador();
        if (tomoCorazon) {
            caverman &jug = nivel->getJugador();
            actualizarBarraVida(jug.getVida());
        }
        // --- 1) Recoger la meta ---
        if (!metaVolcanTomada && metaVolcanItem && cavermanSprite) {
            if (cavermanSprite->collidesWithItem(metaVolcanItem)) {
                metaVolcanTomada = true;

                // Ocultar o eliminar la meta del mapa
                metaVolcanItem->setVisible(false);
                // O si prefieres: scene->removeItem(metaVolcanItem);
                //                delete metaVolcanItem;
                //                metaVolcanItem = nullptr;

                qDebug() << "Meta del volcán tomada";
            }
        }

        // --- 2) Revisar si vuelve a la posición inicial con la meta ---
        if (metaVolcanTomada && !metaVolcanCompletada) {
            caverman &jug = nivel->getJugador();
            float dx = jug.getX() - volcanInicioX;
            float dy = jug.getY() - volcanInicioY;
            // Que flote un poquito encima del caverman
            metaVolcanItem->setVisible(true);
            metaVolcanItem->setPos(
                cavermanSprite->x() + 10,
                cavermanSprite->y() - 40
                );

            // Margen de tolerancia para "llegó al inicio"
            const float tolerancia = 30.0;

            if (std::fabs(dx) < tolerancia && std::fabs(dy) < tolerancia) {
                metaVolcanCompletada = true;
                qDebug() << "Completó objetivo del volcán (recogió meta y volvió al inicio)";

                // Objetivo del volcán completado (trajo la bandera)
                mostrarPantallaResultado(true);
                playerVolcan->stop();
                // O mostrar mensaje de victoria, etc.
            }
        }
    }
    if (tipoNivelActual == TipoNivel::JefeSnow) {

        NivelSnowman *ns = static_cast<NivelSnowman*>(nivel.get());
        if (ns && snowmanSprite) {
            Snowman &boss = ns->getSnowman();
            float temp = ns->getTemperaturaActual();

            // Posición
            snowmanSprite->setPos(boss.getX(), boss.getY());

            // Escala según IA (fase/temperatura)
            float escalaBase = 4.0f;  // la que ya usabas
            snowmanSprite->setScale(escalaBase * boss.getEscalaTamano());

            // --- Decidimos qué frame mostrar ---
            // Suponiendo:
            //  temp > -5   -> termómetro "alto" (frame 0)
            // -10 < temp <= -5 -> medio (frame 1)
            // temp <= -10  -> muy frío (frame 2)

            int index = 0;
            if (temp <= -10.0f) {
                index = 2;
            } else if (temp <= -5.0f) {
                index = 1;
            } else {
                index = 0;
            }

            int srcX = index * termFrameAncho; // 0, 100 o 200
            int srcY = 0;

            QPixmap frame = termometroSheet.copy(
                srcX,
                srcY,
                termFrameAncho,
                termFrameAlto
                );

            termometroItem->setPixmap(frame);
        }

        sincronizarBolasNieveConNivel();

        bool golpeNieve = evaluarColisionBolasNieveConJugador();
        if (golpeNieve) {
            actualizarBarraVida(jug.getVida());
        }
        // --- NUEVO: antorcha y fuego del jugador ---
        sincronizarAntorchaConNivel();
        sincronizarBolitasFuegoConNivel();
        bool golpeBoss = evaluarColisionFuegoConSnowman();
        if (golpeBoss) {
            qDebug() << "Snowman recibe daño. Vida:" << ns->getSnowman().getVida();
        }
        //Muerte del jugador
        if(nivel->getJugador().getVida()==0){
            qDebug() << "El caverman murió";
            mostrarPantallaResultado(false);   // PERDIÓ
        }
        if (static_cast<NivelSnowman*>(nivel.get())->getSnowman().getVida()==0){
            qDebug() << "El snowman murió";
            mostrarPantallaResultado(true);    // GANÓ
        }

    }

    // Actualizar sprite
    if (cavermanSprite) {
        cavermanSprite->setPos(jug.getX(), jug.getY());
    }

    // Posicionar barra de vida en la pantalla
    if (barraVidaItem) {
        QPointF esquina = ui->graphicsView->mapToScene(0, 0);
        float margenX = 40.0;
        float margenY = 40.0;
        barraVidaItem->setPos(esquina.x() + margenX,esquina.y() + margenY);
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
            qDebug() << "Se acabó el tiempo del volcán";
            mostrarPantallaResultado(false);   // PERDIÓ
            if (playerVolcan) playerVolcan->stop();
        }
    }
}

void MainWindow::actualizarBarraVida(int vida)
{
    if (!barraVidaItem) return;
    if (barraVidaSheet.isNull()) return;

    if (vida < 0) vida = 0;
    if (vida > 100) vida = 100;

    int perdida = 100 - vida;
    int fila = perdida / 20;

    if (fila < 0) fila = 0;
    if (fila > 5) fila = 5;

    int srcX = 0;
    int srcY = fila * vidaFrameAlto;

    QPixmap frame = barraVidaSheet.copy(srcX, srcY,vidaFrameAncho,vidaFrameAlto);

    // Borrar la anterior y poner nueva
    barraVidaItem->setPixmap(frame);
}

//colisiones
bool MainWindow::evaluarColisionBolasConJugador()
{
    if (tipoNivelActual != TipoNivel::Volcan) return false;
    if (!cavermanSprite) return false;
    if (!nivel) return false;

    bool huboColision = false;

    const vector<proyectil*> &rocas = nivel->getProyectilesEnemigos();

    size_t limite = min(bolasSprites.size(), rocas.size());

    for (size_t i = 0; i < limite; ++i) {
        QGraphicsPixmapItem *bolaItem = bolasSprites[i];
        if (!bolaItem) continue;

        if (cavermanSprite->collidesWithItem(bolaItem)) {

            caverman &jug = nivel->getJugador();
            jug.recibirDanio(20);
            cavermanSprite->mostrarDanio();

            actualizarBarraVida(jug.getVida());

            // Borrar sprite de la bola
            scene->removeItem(bolaItem);
            delete bolaItem;
            bolasSprites.erase(bolasSprites.begin() + i);

            // Borrar también la roca lógica en el mismo índice
            if (auto nv = dynamic_cast<NivelVolcan*>(nivel.get())) {
                nv->eliminarRocaEnIndice((i));
            }
            huboColision = true;
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

    // Detectar colisión
    if (cavermanSprite->collidesWithItem(mamutSprite))
    {
        colision = true;

        // 1) Lógica del caverman (vida)
        caverman &jug = nivel->getJugador();
        jug.recibirDanio(2);

        // 2) Mostrar daño en el sprite
        cavermanSprite->mostrarDanio();

        // 3) Actualizar barra de corazones
        actualizarBarraVida(jug.getVida());
    }

    return colision;
}
bool MainWindow::evaluarColisionLanzasConMamut()
{
    if (tipoNivelActual != TipoNivel::Mamut || mamutSprite == nullptr) {
        return false;
    }

    NivelMamut *nivelMamut = static_cast<NivelMamut*>(nivel.get());
    if (nivelMamut == nullptr) {
        return false;
    }

    const vector<proyectil*> &lanzasLogicas = nivelMamut->getProyectilesJugador();
    bool huboImpacto = false;

    int i = 0;
    while (i < lanzasSprites.size() && i < lanzasLogicas.size()) {

        QGraphicsPixmapItem *lanzaItem = lanzasSprites[i];

        if (lanzaItem->collidesWithItem(mamutSprite)) {
            huboImpacto = true;

            proyectil *p = lanzasLogicas[i];
            int danio = p->getDanio();
            mamut &boss = nivelMamut->getMamut();
            boss.recibirDanio(danio);
            ui->pbVidaEnemigo->setValue(boss.getVida());

            // Cambiar sprite del mamut según su vida
            if (boss.getVida() <= 0) {
                mamutSprite->matarMamut();
                boss.setVel(0, 0);
                mostrarPantallaResultado(true);

            } else {
                mamutSprite->mostrarDanioMamut();
            }

            // Borrar sprite de la lanza
            scene->removeItem(lanzaItem);
            delete lanzaItem;
            lanzasSprites.erase(lanzasSprites.begin() + i);

            // Borrar la lanza lógica en el mismo índice
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
    if (tipoNivelActual != TipoNivel::JefeSnow) return false;
    if (!cavermanSprite || !nivel) return false;

    NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
    if (!ns) return false;

    bool huboColision = false;

    for (size_t i = 0; i < bolasNieveSprites.size(); ++i) {
        if (cavermanSprite->collidesWithItem(bolasNieveSprites[i])) {

            // 1) eliminar proyectil lógico EN EL NIVEL
            ns->eliminarBolaPorIndice(i);  // asegúrate de tener este método implementado

            // 2) eliminar sprite gráfico
            scene->removeItem(bolasNieveSprites[i]);
            delete bolasNieveSprites[i];
            bolasNieveSprites.erase(bolasNieveSprites.begin() + i);

            // 3) aplicar daño al jugador (daño pequeño)
            caverman &jug = nivel->getJugador();

            int danioBase = 10;
            float mult = 1.0;

            // Usar el agente Snowman para saber cuán fuerte está
            if (ns) {
                Snowman &boss = ns->getSnowman();
                mult = boss.getMultiplicadorDanio();
            }

            int danioFinal = std::max(1, (int)std::round(danioBase * mult));
            jug.recibirDanio(danioFinal);
            cavermanSprite->mostrarDanio();

            // 4) actualizar barra de vida
            actualizarBarraVida(jug.getVida());

            huboColision = true;
            break; // solo 1 golpe por frame
        }
    }

    return huboColision;
}
bool MainWindow::evaluarColisionFlechasConJugador()
{
    if (tipoNivelActual != TipoNivel::Mamut) return false;
    if (!cavermanSprite || !nivel) return false;

    NivelMamut *nm = dynamic_cast<NivelMamut*>(nivel.get());
    if (!nm) return false;

    bool hubo = false;

    for (size_t i = 0; i < bolasSprites.size(); ++i) {
        QGraphicsPixmapItem *item = bolasSprites[i];
        if (!item) continue;

        if (cavermanSprite->collidesWithItem(item)) {

            // 1) decírselo al nivel por índice
            nm->recogerFlechaPorIndice(static_cast<int>(i));

            // 2) quitar el sprite de la escena
            scene->removeItem(item);
            delete item;
            bolasSprites.erase(bolasSprites.begin() + i);

            // (opcional) refrescar contador en el label
            ui->lblMunicion->setText(
                QString::number(nm->getMunicionJugador())
                );

            hubo = true;
            break; // sólo una por frame
        }
    }

    return hubo;
}
bool MainWindow::evaluarColisionCorazonesConJugador()
{
    if (tipoNivelActual != TipoNivel::Volcan) return false;
    if (!cavermanSprite || !nivel) return false;

    NivelVolcan *nv = dynamic_cast<NivelVolcan*>(nivel.get());
    if (!nv) return false;

    caverman &jug = nivel->getJugador();
    bool hubo = false;

    // Recorremos de atrás hacia adelante para borrar sin líos
    for (int i = static_cast<int>(corazonesSprites.size()) - 1; i >= 0; --i) {
        QGraphicsPixmapItem *item = corazonesSprites[i];
        if (!item) continue;

        if (cavermanSprite->collidesWithItem(item)) {
            // 1) Curar vida del jugador
            // Opción segura: usar daño negativo (si tu caverman lo soporta)
            jug.recibirDanio(-20); // suma 20 de vida

            // 2) Actualizar HUD de vida
            actualizarBarraVida(jug.getVida());

            // 3) Eliminar corazón lógico y sprite
            nv->eliminarCorazonEnIndice(i);
            scene->removeItem(item);
            delete item;
            corazonesSprites.erase(corazonesSprites.begin() + i);

            hubo = true;
            // si solo quieres 1 corazón por frame, puedes poner un
            break;
        }
    }

    return hubo;
}
bool MainWindow::evaluarColisionFuegoConSnowman()
{
    if (tipoNivelActual != TipoNivel::JefeSnow) return false;
    if (!snowmanSprite || !nivel) return false;

    NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
    if (!ns) return false;

    bool huboImpacto = false;

    auto &sprites = bolitasFuegoSprites;
    const auto &bolitasLogicas = ns->getBolitasFuego();

    int i = 0;
    while (i < static_cast<int>(sprites.size()) &&
           i < static_cast<int>(bolitasLogicas.size()))
    {
        QGraphicsPixmapItem *item = sprites[i];
        if (!item) { ++i; continue; }

        if (item->collidesWithItem(snowmanSprite)) {

            huboImpacto = true;

            // Lógica del jefe: pierde vida
            Snowman &boss = ns->getSnowman();
            boss.recibirDanio(10);  // ajusta el daño que quieras
            ui->pbVidaEnemigo->setValue(boss.getVida());

            // (Opcional) cambiar animación del snowman a daño
            // snowmanSprite->mostrarDanioSnowman();  // si lo implementas

            // 1) Quitar sprite
            scene->removeItem(item);
            delete item;
            sprites.erase(sprites.begin() + i);

            // 2) Quitar bolita lógica
            ns->eliminarBolitaFuegoEnIndice(i);
            if (boss.getVida() > 0) {
                if (snowmanSprite) {
                    snowmanSprite->mostrarDanioSnowman();
                }
            }
            if (boss.getVida() <= 0) {
                if (snowmanSprite) {
                    snowmanSprite->matarSnowman();
                }
            }



            continue;   // no incrementamos i aquí
        }

        ++i;
    }

    return huboImpacto;
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

    QPixmap bolaPixmap(
        (tipoNivelActual == TipoNivel::Volcan)
            ? QPixmap(":/sprites/bola_fuego.png")
            : QPixmap(":/sprites/lanzas_3.png")
        );
    while (bolasSprites.size() < rocas.size()) {
        QGraphicsPixmapItem* item = scene->addPixmap(bolaPixmap);
        item->setZValue(5);

        if (tipoNivelActual == TipoNivel::Volcan)
            item->setScale(0.4);
        else
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
void MainWindow::sincronizarCorazonesVolcan()
{
    NivelVolcan *nv = dynamic_cast<NivelVolcan*>(nivel.get());
    if (!nv) return;

    const std::vector<proyectil*> &corazones = nv->getCorazones();

    // Ajustar cantidad de sprites
    while (corazonesSprites.size() > corazones.size()) {
        QGraphicsPixmapItem *item = corazonesSprites.back();
        scene->removeItem(item);
        delete item;
        corazonesSprites.pop_back();
    }

    QPixmap corazonPixmap(":/recursos_juego/corazon.PNG"); // pon aquí la ruta de tu sprite
    while (corazonesSprites.size() < corazones.size()) {
        QGraphicsPixmapItem *item = scene->addPixmap(corazonPixmap);
        item->setZValue(6);
        item->setScale(0.5); // ajusta tamaño
        corazonesSprites.push_back(item);
    }

    // Actualizar posiciones
    for (size_t i = 0; i < corazones.size(); ++i) {
        proyectil *c = corazones[i];
        corazonesSprites[i]->setPos(c->getX(), c->getY());
    }
}
void MainWindow::sincronizarLanzasConNivel()
{
    // Nivel debe ser Mamut
    NivelMamut *nivelMamut = dynamic_cast<NivelMamut*>(nivel.get());
    if (!nivelMamut) return;

    const std::vector<proyectil*> &lanzas = nivelMamut->getProyectilesJugador();

    // 1) Ajustar cantidad de sprites
    while (lanzasSprites.size() > lanzas.size()) {
        QGraphicsPixmapItem* item = lanzasSprites.back();
        scene->removeItem(item);
        delete item;
        lanzasSprites.pop_back();
    }

    static QPixmap lanzaPixmap(":/sprites/lanza.png"); // o el recurso correcto

    while (lanzasSprites.size() < lanzas.size()) {
        QGraphicsPixmapItem* item = scene->addPixmap(lanzaPixmap);
        item->setZValue(6);
        item->setScale(1.5);

        // Origen de rotación en el centro del item
        QRectF br = item->boundingRect();
        item->setTransformOriginPoint(br.center());

        lanzasSprites.push_back(item);
    }

    // 2) Actualizar posición y rotación
    for (size_t i = 0; i < lanzas.size(); ++i) {
        proyectil* p = lanzas[i];
        QGraphicsPixmapItem* item = lanzasSprites[i];

        // --- POSICIÓN ---
        // Suponemos que p->getX(), getY() son el CENTRO lógico de la lanza.
        QRectF br = item->boundingRect();
        float w = br.width()  * item->scale();
        float h = br.height() * item->scale();

        item->setPos(p->getX() - w / 2.0f,
                     p->getY() - h / 2.0f);

        // --- ROTACIÓN ---
        float vx = p->getVelX();
        float vy = p->getVelY();

        if (vx == 0.0f && vy == 0.0f) {
            // Sin velocidad, no rotamos (para que no dé basura)
            continue;
        }

        double angRad = std::atan2(vy, vx);
        double angDeg = angRad * 180.0 / 3.14159265;

        // Ajusta este offset según cómo esté dibujada TU lanza en el PNG:
        // - Si la lanza en la imagen apunta a la DERECHA -> offset = 0.0
        // - Si apunta hacia ARRIBA -> offset = -90.0 o +90.0
        const double spriteOffsetDeg = 0.0; // prueba 0.0, -90.0, +90.0

        item->setRotation(angDeg + spriteOffsetDeg);
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
void MainWindow::sincronizarBolitasFuegoConNivel()
{
    if (tipoNivelActual != TipoNivel::JefeSnow) return;

    NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
    if (!ns) return;

    const auto &bolitas = ns->getBolitasFuego();
    size_t total = bolitas.size();

    // 1) Si sobran sprites, borrarlos
    while (bolitasFuegoSprites.size() > total) {
        QGraphicsPixmapItem *item = bolitasFuegoSprites.back();
        scene->removeItem(item);
        delete item;
        bolitasFuegoSprites.pop_back();
    }

    // 2) Si faltan sprites, crearlos
    QPixmap pix(":/sprites/bolita_fuego.png");
    while (bolitasFuegoSprites.size() < total) {
        QGraphicsPixmapItem *item = scene->addPixmap(pix);
        item->setZValue(6);
        item->setScale(0.1);
        bolitasFuegoSprites.push_back(item);
    }

    // 3) Actualizar posiciones (usando (b.x, b.y) como CENTRO)
    for (size_t i = 0; i < bolitas.size(); ++i) {
        const BolitaFuego &b = bolitas[i];
        QGraphicsPixmapItem *item = bolitasFuegoSprites[i];

        QRectF br = item->boundingRect();
        float ancho = br.width()  * item->scale();
        float alto  = br.height() * item->scale();

        // Centrar sprite de la bolita en (b.x, b.y)
        item->setPos(b.x - ancho / 2.0f,
                     b.y - alto  / 2.0f);
    }
}
void MainWindow::sincronizarAntorchaConNivel()
{
    if (tipoNivelActual != TipoNivel::JefeSnow) return;

    NivelSnowman *ns = dynamic_cast<NivelSnowman*>(nivel.get());
    if (!ns) return;

    if (ns->hayAntorchaActiva()) {
        const AntorchaJugador &ant = ns->getAntorcha();

        if (!antorchaItem) {
            QPixmap pix(":/recursos_juego/antorcha_fuego.png");  // cambia al recurso que tengas
            antorchaItem = scene->addPixmap(pix);
            antorchaItem->setZValue(7);
            antorchaItem->setScale(0.1);

            // Origen de transformación al centro para que rote bonito
            antorchaItem->setTransformOriginPoint(
                antorchaItem->boundingRect().center()
            );
        }

        antorchaItem->setVisible(true);

        // --- IMPORTANTE: usar (x,y) como CENTRO ---
        QRectF br = antorchaItem->boundingRect();
        float ancho = br.width()  * antorchaItem->scale();
        float alto  = br.height() * antorchaItem->scale();

        antorchaItem->setPos(
            ant.x - ancho / 2.0f,
            ant.y - alto  / 2.0f
            );

        // Rotación
        antorchaItem->setRotation(ant.angulo);
    } else {
        if (antorchaItem) {
            antorchaItem->setVisible(false);
        }
    }
}

// Nivel Volcán
void MainWindow::on_pushButtonVolcan_clicked()
{
    iniciarNivel(TipoNivel::Volcan);
}
// Nivel Mamut
void MainWindow::on_pushButtonMamut_clicked()
{
    iniciarNivel(TipoNivel::Mamut);
}
// Nivel Snowman
void MainWindow::on_pushButtonSnow_clicked()
{
    iniciarNivel(TipoNivel::JefeSnow);
}
