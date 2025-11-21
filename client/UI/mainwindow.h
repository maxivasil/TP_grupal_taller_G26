#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

#include "../session.h"

#include "readywindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow {
    Q_OBJECT

private:
    const int BASE_WIDTH = 1920;
    const int BASE_HEIGHT = 1080;

    Ui::MainWindow* ui;
    ReadyWindow& readywindow;
    std::string lobbycode;
    std::string car;

    void selector();

    void ready();
    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    explicit MainWindow(ReadyWindow& readywindow, QWidget* parent = nullptr);
    ~MainWindow();

    void connectEvents();

    void updateSession(const std::string& lobby, ClientSession* session);
};
#endif  // MAINWINDOW_H
