#ifndef TRACKWINDOW_H
#define TRACKWINDOW_H

#include <QMainWindow>

#include "mainwindow.h"

class ClientSession;

QT_BEGIN_NAMESPACE
namespace Ui {
class TrackWindow;
}
QT_END_NAMESPACE

class TrackWindow: public QMainWindow {
    Q_OBJECT

public:
    TrackWindow(MainWindow& mainwindow, QWidget* parent = nullptr);
    ~TrackWindow();
    void setSession(ClientSession* session);

private:
    const int BASE_WIDTH = 1920;
    const int BASE_HEIGHT = 1080;

    ClientSession* client_session = nullptr;
    MainWindow& mainwindow;
    Ui::TrackWindow* ui;
    void connectEvents();
    void selectTrack();
    bool eventFilter(QObject* obj, QEvent* event) override;
};
#endif  // TRACKWINDOW_H
