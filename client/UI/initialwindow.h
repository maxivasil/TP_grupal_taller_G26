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
    InitialWindow(MainWindow& mainwindow, TrackWindow& trackwindow, QWidget* parent = nullptr);
    ~InitialWindow();
    void changeScreen(const std::string& lobby);
    void showError();
    void setSession(ClientSession* session);

private:
    const int BASE_WIDTH = 1920;
    const int BASE_HEIGHT = 1080;

    ClientSession* client_session = nullptr;
    MainWindow& mainwindow;
    TrackWindow& trackwindow;
    Ui::InitialWindow* ui;
    bool created;
    void connectEvents();
    void joinLobby();
    void createLobby();
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};
#endif  // INITIALWINDOW_H
