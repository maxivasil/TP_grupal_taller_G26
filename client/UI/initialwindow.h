#ifndef INITIALWINDOW_H
#define INITIALWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"

class ClientSession;

QT_BEGIN_NAMESPACE
namespace Ui
{
class InitialWindow;
}
QT_END_NAMESPACE

class InitialWindow : public QMainWindow {
	Q_OBJECT

    public:
	InitialWindow(ClientSession& client_session, MainWindow& mainwindow, QWidget *parent = nullptr);
	~InitialWindow();
	void changeScreen(std::string lobby);
	void showError();

    private:
	ClientSession& client_session;
	MainWindow& mainwindow;
	Ui::InitialWindow *ui;
	void connectEvents();
	void joinLobby();
	void createLobby();

};
#endif // INITIALWINDOW_H
