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
    TrackWindow(ClientSession& client_session, MainWindow& mainwindow, QWidget* parent = nullptr);
    ~TrackWindow();

private:
    ClientSession& client_session;
    MainWindow& mainwindow;
    Ui::TrackWindow* ui;
    void connectEvents();
    void selectTrack();
};
#endif  // TRACKWINDOW_H
