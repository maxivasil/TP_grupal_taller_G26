#include "initialwindow.h"

#include <QGuiApplication>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../cmd/client_to_server_lobby.h"
#include "../cmd/server_to_client_lobbyResponse.h"
#include "../session.h"
#include "./ui_initialwindow.h"

InitialWindow::InitialWindow(ClientSession& client_session, MainWindow& mainwindow,
                             TrackWindow& trackwindow, QWidget* parent):
        QMainWindow(parent),
        client_session(client_session),
        mainwindow(mainwindow),
        trackwindow(trackwindow),
        ui(new Ui::InitialWindow),
        created(false) {
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

void InitialWindow::joinLobby() {
    QLineEdit* lobby_line = findChild<QLineEdit*>("lobby");
    std::string lobby = (lobby_line->text()).toStdString();
    if (lobby.size() != 6) {
        QLabel* labelOut = findChild<QLabel*>("intro_text");
        labelOut->setText("El Lobby debe tener exactamente 6 caracteres");
    } else {
        client_session.send_command(new ClientToServerLobby(lobby, false));
        ServerToClientCmd_Client* raw_cmd;
        Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();
        std::vector<ServerToClientCmd_Client*> stash;
        while (true) {
            if (recv_queue.try_pop(raw_cmd)) {
                std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
                auto* response_cmd = dynamic_cast<ServerToClientLobbyResponse*>(cmd.get());
                if (response_cmd) {
                    ClientContext ctx = {.game = nullptr, .mainwindow = (this)};
                    response_cmd->execute(ctx);
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
}

void InitialWindow::createLobby() {
    client_session.send_command(new ClientToServerLobby("AAAAAA", true));
    created = true;
    ServerToClientCmd_Client* raw_cmd;
    Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();
    std::vector<ServerToClientCmd_Client*> stash;
    while (true) {
        if (recv_queue.try_pop(raw_cmd)) {
            std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
            auto* response_cmd = dynamic_cast<ServerToClientLobbyResponse*>(cmd.get());
            if (response_cmd) {
                ClientContext ctx = {.game = nullptr, .mainwindow = (this)};
                response_cmd->execute(ctx);
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

void InitialWindow::changeScreen(const std::string& lobby) {
    this->hide();
    mainwindow.updateLobby(lobby);
    if (created) {
        trackwindow.show();
    } else {
        mainwindow.show();
    }
}

void InitialWindow::showError() {
    QLabel* labelOut = findChild<QLabel*>("intro_text");
    labelOut->setText("Error al intentar unirse a la partida Compruebe que el código sea correcto");
    created = false;
}

void InitialWindow::connectEvents() {
    const QPushButton* lobby = findChild<QPushButton*>("joinlobby");
    QObject::connect(lobby, &QPushButton::clicked, this, &InitialWindow::joinLobby);

    const QPushButton* createlobby = findChild<QPushButton*>("newlobby");
    QObject::connect(createlobby, &QPushButton::clicked, this, &InitialWindow::createLobby);
}

InitialWindow::~InitialWindow() { delete ui; }
