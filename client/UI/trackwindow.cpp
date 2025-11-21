#include "trackwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <algorithm>
#include <memory>
#include <string>

#include "../cmd/client_to_server_tour.h"
#include "../session.h"
#include "./ui_trackwindow.h"

TrackWindow::TrackWindow(MainWindow& mainwindow, QWidget* parent):
        QMainWindow(parent), mainwindow(mainwindow), ui(new Ui::TrackWindow) {

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

    QLabel* title = findChild<QLabel*>("intro_text_2");
    if (title) {
        title->setFont(title->font());
        title->setWordWrap(false);

        QRect orig = title->property("origGeometry").toRect();
        float scale = scaleFactor;

        int scaledWidth = orig.width() * scale;
        int scaledHeight = orig.height() * scale;

        int posX = (this->width() - scaledWidth) / 2;
        int posY = orig.y() * scale;

        title->setGeometry(posX, posY, scaledWidth, scaledHeight);
        title->setAlignment(Qt::AlignCenter);
    }

    connectEvents();
}

void TrackWindow::selectTrack() {
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton)
        return;

    std::string tourFile = senderButton->property("track_souce").toString().toStdString();
    client_session->send_command(new ClientToServerTour(tourFile));

    this->hide();
    mainwindow.show();
}

void TrackWindow::connectEvents() {
    auto connectButton = [&](QString name, const char* tour) {
        if (QPushButton* btn = findChild<QPushButton*>(name)) {
            btn->setProperty("track_souce", tour);
            QObject::connect(btn, &QPushButton::clicked, this, &TrackWindow::selectTrack);
        }
    };

    connectButton("begginer", "tours/tour1.yaml");
    connectButton("intermediate", "tours/tour2.yaml");
    connectButton("expert", "tours/tour3.yaml");
}

bool TrackWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == centralWidget() && event->type() == QEvent::Paint) {
        QPainter painter(centralWidget());
        QPixmap bg(":/new/prefix1/Assets/Trackscreen.png");
        bg = bg.scaled(centralWidget()->size(), Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, bg);
    }
    return false;
}

void TrackWindow::setSession(ClientSession* session) { client_session = session; }

TrackWindow::~TrackWindow() { delete ui; }
