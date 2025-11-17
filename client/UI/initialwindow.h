#ifndef INITIALWINDOW_H
#define INITIALWINDOW_H

#include <QMainWindow>
#include <string>

#include "mainwindow.h"
#include "trackwindow.h"

class ClientSession;

QT_BEGIN_NAMESPACE
namespace Ui {
class InitialWindow;
}
QT_END_NAMESPACE

class InitialWindow: public QMainWindow {
    Q_OBJECT

public:
    InitialWindow(ClientSession& client_session, MainWindow& mainwindow, TrackWindow& trackwindow,
                  QWidget* parent = nullptr);
    ~InitialWindow();
    void changeScreen(const std::string& lobby);
    void showError();

private:
    ClientSession& client_session;
    MainWindow& mainwindow;
    TrackWindow& trackwindow;
    Ui::InitialWindow* ui;
    bool created;
    void connectEvents();
    void joinLobby();
    void createLobby();
};
#endif  // INITIALWINDOW_H
