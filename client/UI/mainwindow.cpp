#include "mainwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
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
    this->setWindowIcon(QIcon(":/new/prefix1/Assets/logo.png"));

    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->availableGeometry().size();

    int width = screenSize.width();
    int height = screenSize.height();

    if (width * 9 != height * 16) {
        if ((float)width / height > 16.0 / 9.0) {
            width = height * 16 / 9;
        } else {
            height = width * 9 / 16;
        }
    }
    setFixedSize(width, height);
    move((screenSize.width() - width) / 2, (screenSize.height() - height) / 2);

    QPixmap bg(":/new/prefix1/Assets/Background.png");
    bg = bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPalette palette;
    palette.setBrush(QPalette::Window, bg);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
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
    QLabel* labelOut = findChild<QLabel*>("intro_text");
    QString carName = senderButton->property("car_name").toString();

    // Obtener estadísticas del auto
    CarStats stats = CarStatsDatabase::getCarStats(carName.toStdString());

    // Formatear las estadísticas para mostrar
    std::ostringstream oss;
    oss << "🚗 " << carName.toStdString() << "\n\n";
    oss << std::fixed << std::setprecision(1);
    oss << "⚡ Velocidad Máxima: " << stats.max_speed << " km/h\n";
    oss << "🏃 Aceleración: " << stats.acceleration << " m/s²\n";
    oss << "❤️  Salud: " << stats.health_max << " HP\n";
    oss << "⚖️  Masa: " << stats.mass << " kg\n";
    oss << "🎮 Controlabilidad: " << stats.handling << "/5\n";

    labelOut->setText(QString::fromStdString(oss.str()));
    car = carName.toStdString();
}

void MainWindow::updateLobby(const std::string& lobby) {
    lobbycode = lobby;
    QLabel* labelOut = findChild<QLabel*>("Lobby_text");
    labelOut->setText("Código de Sala: " + QString::fromStdString(lobby));
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

MainWindow::~MainWindow() { delete ui; }
