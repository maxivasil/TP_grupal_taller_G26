#ifndef READYWINDOW_H
#define READYWINDOW_H

#include <QMainWindow>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class ReadyWindow;
}
QT_END_NAMESPACE

class ClientSession;

class ReadyWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit ReadyWindow(ClientSession& client_session, QWidget* parent = nullptr);
    ~ReadyWindow();
    void updateLobby(const std::string& lobby, const std::string& new_car);

private:
    const int BASE_WIDTH = 1920;
    const int BASE_HEIGHT = 1080;

    Ui::ReadyWindow* ui;
    ClientSession& client_session;
    std::string car;
    void connectEvents();
    void setReady();
    bool eventFilter(QObject* obj, QEvent* event) override;
};
#endif  // READYWINDOW_H
