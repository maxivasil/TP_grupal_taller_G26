#include "readywindow.h"

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

#include "../cmd/client_to_server_readyToStart_client.h"
#include "../cmd/server_to_client_gameStarting_client.h"
#include "../session.h"
#include "./ui_readywindow.h"

ReadyWindow::ReadyWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::ReadyWindow) {
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
    Queue<ServerToClientCmd_Client*>& recv_queue = client_session->get_recv_queue();
    client_session->send_command(new ClientToServerReady_Client(car, username));
    std::vector<ServerToClientCmd_Client*> stash;
    while (true) {
        ServerToClientCmd_Client* raw_cmd = recv_queue.pop();
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
    for (auto* c: stash) {
        recv_queue.push(c);
    }
}

void ReadyWindow::updateLobby(const std::string& lobby, const std::string& new_car) {
    car = new_car;
    QLabel* lobbyLabel = findChild<QLabel*>("Lobby_text");
    if (lobbyLabel) {
        lobbyLabel->setProperty("doNotScale", true);
        QFont font = lobbyLabel->font();
        font.setPointSizeF(28);
        lobbyLabel->setFont(font);

        lobbyLabel->setText("Código de Sala: " + QString::fromStdString(lobby));
    }
}

bool ReadyWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == centralWidget() && event->type() == QEvent::Paint) {
        QPainter painter(centralWidget());
        QPixmap bg(":/new/prefix1/Assets/Background.png");
        bg = bg.scaled(centralWidget()->size(), Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, bg);
    }
    return false;
}

void ReadyWindow::setName(const std::string& name) { username = name; }

void ReadyWindow::setSession(ClientSession* session) { client_session = session; }

ReadyWindow::~ReadyWindow() { delete ui; }
