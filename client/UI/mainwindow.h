#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class ClientSession;

class MainWindow : public QMainWindow {
	Q_OBJECT

    private:
	ClientSession& client_session;
	Ui::MainWindow *ui;
	std::string car;

	void selector();

	void ready();

    public:
	MainWindow(ClientSession& client_session, QWidget *parent = nullptr);
	~MainWindow();

	void connectEvents();

};
#endif // MAINWINDOW_H
