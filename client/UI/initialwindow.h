#ifndef INITIALWINDOW_H
#define INITIALWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class InitialWindow;
}
QT_END_NAMESPACE

class InitialWindow : public QMainWindow {
	Q_OBJECT

    public:
	InitialWindow(QWidget *parent = nullptr);
	~InitialWindow();


    private:
	Ui::InitialWindow *ui;
	void connectEvents();
	void joinlobby();
};
#endif // INITIALWINDOW_H
