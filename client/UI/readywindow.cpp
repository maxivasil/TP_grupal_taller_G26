#include "readywindow.h"

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

#include "../cmd/client_to_server_readyToStart_client.h"
#include "../cmd/server_to_client_gameStarting_client.h"
#include "../session.h"
#include "./ui_readywindow.h"

ReadyWindow::ReadyWindow(ClientSession& client_session, QWidget* parent):
        QMainWindow(parent), ui(new Ui::ReadyWindow), client_session(client_session) {
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

void ReadyWindow::connectEvents() {
    const QPushButton* ready = findChild<QPushButton*>("Ready");
    QObject::connect(ready, &QPushButton::clicked, this, &ReadyWindow::setReady);
}

void ReadyWindow::setReady() {
    QLabel* labelOut = findChild<QLabel*>("newText");
    labelOut->setText("Esperando al resto de jugadores...");
    labelOut->repaint();
    QApplication::processEvents();
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    senderButton->setEnabled(false);
    ServerToClientCmd_Client* raw_cmd;
    Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();
    client_session.send_command(new ClientToServerReady_Client(car));
    std::vector<ServerToClientCmd_Client*> stash;
    while (true) {
        if (recv_queue.try_pop(raw_cmd)) {
            std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
            auto* start_cmd = dynamic_cast<ServerToClientGameStarting_Client*>(cmd.get());
            if (start_cmd) {
                ClientContext ctx = {.game = nullptr, .mainwindow = (this)};
                start_cmd->execute(ctx);
                break;
            } else {
                stash.push_back(cmd.release());
            }
        }
    }
    for (auto* c: stash) {
        recv_queue.push(c);
    }
}

void ReadyWindow::updateLobby(const std::string& lobby, const std::string& new_car) {
    car = new_car;
    QLabel* labelOut = findChild<QLabel*>("Lobby_text");
    labelOut->setText("Código de Sala: " + QString::fromStdString(lobby));
}


ReadyWindow::~ReadyWindow() { delete ui; }
