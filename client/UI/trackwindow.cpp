#include "trackwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "../cmd/client_to_server_tour.h"
#include "../session.h"
#include "./ui_trackwindow.h"

TrackWindow::TrackWindow(ClientSession& client_session, MainWindow& mainwindow, QWidget* parent):
        QMainWindow(parent),
        client_session(client_session),
        mainwindow(mainwindow),
        ui(new Ui::TrackWindow) {
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

    QPixmap bg(":/new/prefix1/Assets/Trackscreen.png");
    bg = bg.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPalette palette;
    palette.setBrush(QPalette::Window, bg);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    connectEvents();
}

void TrackWindow::selectTrack() {
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    std::string tourFile = senderButton->property("track_souce").toString().toStdString();
    client_session.send_command(new ClientToServerTour(tourFile));
    this->hide();
    mainwindow.show();
}

void TrackWindow::connectEvents() {
    QPushButton* begginer = findChild<QPushButton*>("begginer");
    begginer->setProperty("track_souce", "tours/tour1.yaml");
    QObject::connect(begginer, &QPushButton::clicked, this, &TrackWindow::selectTrack);

    QPushButton* intermediate = findChild<QPushButton*>("intermediate");
    intermediate->setProperty("track_souce", "tours/tour2.yaml");
    QObject::connect(intermediate, &QPushButton::clicked, this, &TrackWindow::selectTrack);

    QPushButton* expert = findChild<QPushButton*>("expert");
    expert->setProperty("track_souce", "tours/tour3.yaml");
    QObject::connect(expert, &QPushButton::clicked, this, &TrackWindow::selectTrack);
}


TrackWindow::~TrackWindow() { delete ui; }
