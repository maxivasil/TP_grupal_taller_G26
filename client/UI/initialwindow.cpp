#include "initialwindow.h"
#include "./ui_initialwindow.h"
#include <string>
#include <QPixmap>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include <QScreen>
#include <QGuiApplication>

#include "../session.h"
#include "../cmd/client_to_server_lobby.h"
#include "../cmd/server_to_client_lobbyResponse.h"

InitialWindow::InitialWindow(ClientSession& client_session, MainWindow& mainwindow, QWidget *parent)
	: QMainWindow(parent)
	, client_session(client_session)
	, mainwindow(mainwindow)
	, ui(new Ui::InitialWindow)
{
	ui->setupUi(this);
	QScreen *screen = QGuiApplication::primaryScreen();
	QSize screenSize = screen->availableGeometry().size();

	int width = screenSize.width();
	int height = screenSize.height();

	if (width * 9 != height * 16) {
		if ((float)width / height > 16.0/9.0) {
			width = height * 16 / 9;
		} else {
			height = width * 9 / 16;
		}
	}
	setFixedSize(width, height);
	move((screenSize.width() - width)/2, (screenSize.height() - height)/2);

	QPixmap bg(":/new/prefix1/Assets/Background.png");
	bg = bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	QPalette palette;
	palette.setBrush(QPalette::Window, bg);
	this->setPalette(palette);
	this->setAutoFillBackground(true);
	connectEvents();
}

void InitialWindow::joinLobby(){
	QLineEdit* lobby_line = findChild<QLineEdit*>("lobby");
	std::string lobby = (lobby_line->text()).toStdString();
	if(lobby.size() != 6){
		QLabel* labelOut = findChild<QLabel*>("intro_text");
		labelOut->setText("El Lobby debe tener exactamente 6 caracteres");
	}
	else{
		client_session.send_command(new ClientToServerLobby(lobby, false));
		ServerToClientCmd_Client* raw_cmd;
		Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();
		while (true) {
			if (recv_queue.try_pop(raw_cmd)) {
				std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
				auto* response_cmd = dynamic_cast<ServerToClientLobbyResponse*>(cmd.get());
				if (response_cmd) {
					ClientContext ctx = {.game = nullptr, .mainwindow = (this)};
					response_cmd->execute(ctx);
					break;
				}
			}
		}
	}
}

void InitialWindow::createLobby(){
	client_session.send_command(new ClientToServerLobby("AAAAAA", true));
	ServerToClientCmd_Client* raw_cmd;
	Queue<ServerToClientCmd_Client*>& recv_queue = client_session.get_recv_queue();
	while (true) {
		if (recv_queue.try_pop(raw_cmd)) {
			std::unique_ptr<ServerToClientCmd_Client> cmd(raw_cmd);
			auto* response_cmd = dynamic_cast<ServerToClientLobbyResponse*>(cmd.get());
			if (response_cmd) {
				ClientContext ctx = {.game = nullptr, .mainwindow = (this)};
				response_cmd->execute(ctx);
				break;
			}
		}	
	}
}

void InitialWindow::changeScreen(std::string lobby){
	this->hide();
	mainwindow.show();
	mainwindow.updateLobby(lobby);
}

void InitialWindow::showError(){
	QLabel* labelOut = findChild<QLabel*>("intro_text");
	labelOut->setText("Error al intentar unirse a la partida \n Compruebe que el código sea correcto o no esté en marcha");
}

void InitialWindow::connectEvents() {
	QPushButton* lobby = findChild<QPushButton*>("joinlobby");
	QObject::connect(lobby, &QPushButton::clicked,
			 this, &InitialWindow::joinLobby);
		
	QPushButton* createlobby = findChild<QPushButton*>("newlobby");
	QObject::connect(createlobby, &QPushButton::clicked,
			 this, &InitialWindow::createLobby);
}

InitialWindow::~InitialWindow()
{
	delete ui;
}
