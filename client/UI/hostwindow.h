#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <QMainWindow>
#include <optional>
#include "initialwindow.h"
#include "../session.h"
#include "../../common/queue.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class HostWindow;
}
QT_END_NAMESPACE

class HostWindow : public QMainWindow {
	Q_OBJECT

    public:
	HostWindow(Queue<ServerToClientCmd_Client*>& queue, InitialWindow& initialwindow, QWidget *parent = nullptr);
	~HostWindow();
	ClientSession& getSession();

    private:
	const int BASE_WIDTH = 1920;
    const int BASE_HEIGHT = 1080;
	Ui::HostWindow *ui;
	std::optional<ClientSession> client_session;
	Queue<ServerToClientCmd_Client*>& queue;
	InitialWindow& initialwindow;

	void connectEvents();
    void createConection();
	bool eventFilter(QObject* obj, QEvent* event) override;
};
#endif // HOSTWINDOW_H
