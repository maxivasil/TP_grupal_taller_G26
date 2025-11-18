#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

#include "readywindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    ReadyWindow& readywindow;
    std::string lobbycode;
    std::string car;

    void selector();

    void ready();

public:
    explicit MainWindow(ReadyWindow& readywindow, QWidget* parent = nullptr);
    ~MainWindow();

    void connectEvents();

    void updateLobby(const std::string& lobby);
};
#endif  // MAINWINDOW_H
