#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../session.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

    private:
	ClientSession& client_session;
	Ui::MainWindow *ui;

	void selector();

    public:
	MainWindow(ClientSession& client_session, QWidget *parent = nullptr);
	~MainWindow();

	void connectEvents();

};
#endif // MAINWINDOW_H
