#include "mainwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "../../common/CarStats.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(ReadyWindow& readywindow, QWidget* parent):
        QMainWindow(parent), ui(new Ui::MainWindow), readywindow(readywindow) {
    ui->setupUi(this);
    this->setWindowState(Qt::WindowFullScreen);
    this->setWindowIcon(QIcon(":/new/prefix1/Assets/logo.png"));

    QWidget* cw = centralWidget();
    cw->installEventFilter(this);

    QWidget* root = this;
    QList<QWidget*> widgets = root->findChildren<QWidget*>();

    for (QWidget* w: widgets) {
        w->setProperty("origGeometry", w->geometry());
        w->setProperty("origFontSize", w->font().pointSizeF());
    }

    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->availableGeometry().size();

    float scaleX = (float)screenSize.width() / BASE_WIDTH;
    float scaleY = (float)screenSize.height() / BASE_HEIGHT;
    float scaleFactor = std::min(scaleX, scaleY);

    for (QWidget* w: widgets) {
        if (w->property("doNotScale").toBool())
            continue;

        w->resize(w->width() * scaleFactor, w->height() * scaleFactor);
        w->move(w->x() * scaleFactor, w->y() * scaleFactor);

        QFont font = w->font();
        float originalSize = w->property("origFontSize").toFloat();
        if (originalSize > 0) {
            font.setPointSizeF(originalSize * scaleFactor);
        } else {
            font.setPointSizeF(12 * scaleFactor);
        }
        w->setFont(font);
    }

    QLabel* title = findChild<QLabel*>("intro_text");
    if (title) {
        int origHeight = title->property("origGeometry").toRect().height();
        title->setGeometry(0, height() * 0.35, width(), origHeight);
        title->setAlignment(Qt::AlignCenter);
    }
    connectEvents();
}

void MainWindow::ready() {
    QLabel* labelOut = findChild<QLabel*>("intro_text");
    labelOut->setText("Has seleccionado un " + QString::fromStdString(car) +
                      "\n\n Esperando al resto de jugadores...");
    readywindow.updateLobby(lobbycode, car);
    this->hide();
    readywindow.show();
}


void MainWindow::selector() {
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    QLabel* statsLabel = findChild<QLabel*>("intro_text");
    if (!statsLabel)
        return;

    QString carName = senderButton->property("car_name").toString();
    CarStats stats = CarStatsDatabase::getCarStats(carName.toStdString());

    std::ostringstream oss;
    oss << carName.toStdString() << "\n"
        << "------------------------------\n"
        << "Velocidad Máxima: " << stats.max_speed << " km/h\n"
        << "Aceleración:      " << stats.acceleration << " m/s²\n"
        << "Salud:            " << stats.health_max << " HP\n"
        << "Masa:             " << stats.mass << " kg\n"
        << "Controlabilidad:  " << stats.handling << " / 5";

    statsLabel->setTextFormat(Qt::PlainText);
    statsLabel->setText(QString::fromStdString(oss.str()));
    statsLabel->setWordWrap(true);
    statsLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    int boxWidth = width() * 0.50;
    int boxHeight = height() * 0.35;
    int posX = (width() - boxWidth) / 2;
    int posY = height() * 0.60;

    statsLabel->setGeometry(posX, posY, boxWidth, boxHeight);

    statsLabel->setStyleSheet("QLabel {"
                              "   background-color: rgba(0, 0, 0, 130);"
                              "   border: 2px solid white;"
                              "   border-radius: 14px;"
                              "   color: white;"
                              "   padding: 20px;"
                              "   font-size: 26px;"
                              "   font-family: 'DejaVu Sans Mono';"
                              "}");

    QLabel* carBox = findChild<QLabel*>("label_2");
    if (carBox) {
        int carWidth = carBox->width();
        int carPosX = (width() - carWidth) / 2;
        carBox->move(carPosX, carBox->y());
    }

    car = carName.toStdString();

    car = carName.toStdString();
}

void MainWindow::updateLobby(const std::string& lobby) {
    lobbycode = lobby;
    QLabel* lobbyLabel = findChild<QLabel*>("Lobby_text");
    if (lobbyLabel) {
        lobbyLabel->setProperty("doNotScale", true);
        QFont font = lobbyLabel->font();
        font.setPointSizeF(28);
        lobbyLabel->setFont(font);

        lobbyLabel->setText("Código de Sala: " + QString::fromStdString(lobby));
    }
}


void MainWindow::connectEvents() {
    const QPushButton* readyButton = findChild<QPushButton*>("Ready");
    QObject::connect(readyButton, &QPushButton::clicked, this, &MainWindow::ready);

    QPushButton* celesteButton = findChild<QPushButton*>("Celeste");
    celesteButton->setProperty("car_name", "Chevrolet Corsa");
    QObject::connect(celesteButton, &QPushButton::clicked, this, &MainWindow::selector);

    QPushButton* descapotableButton = findChild<QPushButton*>("Descapotable");
    descapotableButton->setProperty("car_name", "Lamborghini Diablo");
    QObject::connect(descapotableButton, &QPushButton::clicked, this, &MainWindow::selector);

    QPushButton* ferrariButton = findChild<QPushButton*>("Ferrari");
    ferrariButton->setProperty("car_name", "Dodge Viper");
    QObject::connect(ferrariButton, &QPushButton::clicked, this, &MainWindow::selector);

    QPushButton* jeepButton = findChild<QPushButton*>("Jeep");
    jeepButton->setProperty("car_name", "Jeep Wrangler");
    QObject::connect(jeepButton, &QPushButton::clicked, this, &MainWindow::selector);

    QPushButton* limoButton = findChild<QPushButton*>("Limo");
    limoButton->setProperty("car_name", "Lincoln TownCar");
    QObject::connect(limoButton, &QPushButton::clicked, this, &MainWindow::selector);

    QPushButton* pickupButton = findChild<QPushButton*>("Pickup");
    pickupButton->setProperty("car_name", "Toyota Tacoma");
    QObject::connect(pickupButton, &QPushButton::clicked, this, &MainWindow::selector);

    QPushButton* vanButton = findChild<QPushButton*>("Van");
    vanButton->setProperty("car_name", "Iveco Daily");
    QObject::connect(vanButton, &QPushButton::clicked, this, &MainWindow::selector);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == centralWidget() && event->type() == QEvent::Paint) {
        QPainter painter(centralWidget());
        QPixmap bg(":/new/prefix1/Assets/Background.png");
        bg = bg.scaled(centralWidget()->size(), Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, bg);
    }
    return false;
}

MainWindow::~MainWindow() { delete ui; }
