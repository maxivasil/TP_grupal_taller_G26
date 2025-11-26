#include "initialwindow.h"

#include <QGuiApplication>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../cmd/client_to_server_joinLobby_client.h"
#include "../cmd/server_to_client_joinLobbyResponse_client.h"
#include "../session.h"
#include "./ui_initialwindow.h"

InitialWindow::InitialWindow(MainWindow& mainwindow, TrackWindow& trackwindow, QWidget* parent):
        QMainWindow(parent),
        mainwindow(mainwindow),
        trackwindow(trackwindow),
        ui(new Ui::InitialWindow),
        created(false) {
    ui->setupUi(this);
    this->setWindowState(Qt::WindowFullScreen);

    QLabel* title = findChild<QLabel*>("intro_text_2");
    if (title) {
        title->setGeometry(0, title->y(), this->width(), title->height());
        title->setAlignment(Qt::AlignCenter);
    }

    QWidget* root = this;
    QList<QWidget*> widgets = root->findChildren<QWidget*>();

    for (QWidget* w: widgets) {
        w->setProperty("origGeometry", w->geometry());
        w->setProperty("origFontSize", w->font().pointSizeF());
    }

    this->setWindowIcon(QIcon(":/new/prefix1/Assets/logo.png"));

    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->availableGeometry().size();

    float scaleX = (float)screenSize.width() / BASE_WIDTH;
    float scaleY = (float)screenSize.height() / BASE_HEIGHT;
    float scaleFactor = std::min(scaleX, scaleY);

    for (QWidget* w: widgets) {
        w->resize(w->width() * scaleFactor, w->height() * scaleFactor);
        w->move(w->x() * scaleFactor, w->y() * scaleFactor);

        QFont font = w->font();

        int origPx = font.pixelSize();
        if (origPx > 0) {
            font.setPixelSize(origPx * scaleFactor);
        } else {
            float origPt = font.pointSizeF();
            if (origPt <= 0)
                origPt = 12;
            font.setPointSizeF(origPt * scaleFactor);
        }

        w->setFont(font);
    }
    connectEvents();
}

void InitialWindow::joinLobby() {
    QLineEdit* lobby_line = findChild<QLineEdit*>("lobby");
    std::string lobby = (lobby_line->text()).toStdString();
    if (lobby.size() != 6) {
        QLabel* labelOut = findChild<QLabel*>("intro_text");
        labelOut->setText("El Lobby debe tener exactamente 6 caracteres");
    } else {
        client_session->send_command(new ClientToServerJoinLobby_Client(lobby, false));
        ServerToClientCmd_Client* raw_cmd;
        Queue<ServerToClientCmd_Client*>& recv_queue = client_session->get_recv_queue();
        std::vector<ServerToClientCmd_Client*> stash;
        while (true) {
            if (recv_queue.try_pop(raw_cmd)) {
                std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
                auto* response_cmd =
                        dynamic_cast<ServerToClientJoinLobbyResponse_Client*>(cmd.get());
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
    client_session->send_command(new ClientToServerJoinLobby_Client("AAAAAA", true));
    created = true;
    ServerToClientCmd_Client* raw_cmd;
    Queue<ServerToClientCmd_Client*>& recv_queue = client_session->get_recv_queue();
    std::vector<ServerToClientCmd_Client*> stash;
    while (true) {
        if (recv_queue.try_pop(raw_cmd)) {
            std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
            auto* response_cmd = dynamic_cast<ServerToClientJoinLobbyResponse_Client*>(cmd.get());
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
    mainwindow.updateSession(lobby, client_session);
    trackwindow.setSession(client_session);
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

void InitialWindow::showEvent(QShowEvent*) {
    QWidget* cw = centralWidget();
    cw->installEventFilter(this);
}

bool InitialWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == centralWidget() && event->type() == QEvent::Paint) {
        QPainter painter(centralWidget());

        QPixmap bg(":/new/prefix1/Assets/Background.png");
        bg = bg.scaled(centralWidget()->size(), Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, bg);
    }
    return false;
}

void InitialWindow::setSession(ClientSession* session) { client_session = session; }

InitialWindow::~InitialWindow() { delete ui; }
