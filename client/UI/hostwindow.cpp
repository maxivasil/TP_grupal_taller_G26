#include "hostwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <algorithm>
#include <exception>
#include <string>

#include "./ui_hostwindow.h"

HostWindow::HostWindow(Queue<ServerToClientCmd_Client*>& queue, InitialWindow& initialwindow,
                       ReadyWindow& readywindow, QWidget* parent):
        QMainWindow(parent),
        ui(new Ui::HostWindow),
        queue(queue),
        initialwindow(initialwindow),
        readywindow(readywindow) {
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
    connectEvents();
}

void HostWindow::createConection() {
    QTextEdit* username = findChild<QTextEdit*>("Username");
    std::string username_text = (username->toPlainText()).toStdString();
    QTextEdit* ip = findChild<QTextEdit*>("IP");
    std::string ip_text = (ip->toPlainText()).toStdString();
    QTextEdit* puerto = findChild<QTextEdit*>("Puerto");
    std::string puerto_text = (puerto->toPlainText()).toStdString();
    if (username_text.size() == 0 || ip_text.size() == 0 || puerto_text.size() == 0) {
        QLabel* error_text = findChild<QLabel*>("intro_text");
        error_text->setText("Se deben rellenar todos los campos!");
        return;
    }
    try {
        client_session.emplace(ip_text.c_str(), puerto_text.c_str(), queue);
        client_session->start();
        this->hide();
        initialwindow.show();
        initialwindow.setSession(&client_session.value());
        readywindow.setName(username_text);
    } catch (...) {
        QLabel* error_text = findChild<QLabel*>("intro_text");
        error_text->setText("Error al intentar conectarse");
    }
}

void HostWindow::connectEvents() {
    const QPushButton* bcontinue = findChild<QPushButton*>("bcontinue");
    QObject::connect(bcontinue, &QPushButton::clicked, this, &HostWindow::createConection);
}

ClientSession& HostWindow::getSession() { return client_session.value(); }

bool HostWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == centralWidget() && event->type() == QEvent::Paint) {
        QPainter painter(centralWidget());
        QPixmap bg(":/new/prefix1/Assets/Background.png");
        bg = bg.scaled(centralWidget()->size(), Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, bg);
    }
    return false;
}

HostWindow::~HostWindow() { delete ui; }
