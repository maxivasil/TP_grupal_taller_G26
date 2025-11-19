#include "hostwindow.h"
#include "./ui_hostwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <string>

#include <exception>

HostWindow::HostWindow(Queue<ServerToClientCmd_Client*>& queue, InitialWindow& initialwindow, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::HostWindow)
	, queue(queue)
	, initialwindow(initialwindow)
{
	ui->setupUi(this);
	this->setWindowIcon(QIcon(":/new/prefix1/Assets/logo.png"));

	QScreen* screen = QGuiApplication::primaryScreen();
	QSize screenSize = screen->availableGeometry().size();

	int width = screenSize.width();
	int height = screenSize.height();

	if (width * 9 != height * 16) {
		if ((float)width / height > 16.0 / 9.0) {
			width = height * 16 / 9;
		} else {
			height = width * 9 / 16;
		}
	}
	setFixedSize(width, height);
	move((screenSize.width() - width) / 2, (screenSize.height() - height) / 2);

	QPixmap bg(":/new/prefix1/Assets/Background.png");
	bg = bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	QPalette palette;
	palette.setBrush(QPalette::Window, bg);
	this->setPalette(palette);
	this->setAutoFillBackground(true);
	connectEvents();
}

void HostWindow::createConection(){
	QTextEdit* username = findChild<QTextEdit*>("Username");
	std::string username_text = (username->toPlainText()).toStdString();
	QTextEdit* ip = findChild<QTextEdit*>("IP");
	std::string ip_text = (ip->toPlainText()).toStdString();
	QTextEdit* puerto = findChild<QTextEdit*>("Puerto");
	std::string puerto_text = (puerto->toPlainText()).toStdString();
	if (username_text.size()==0 || ip_text.size()==0 || puerto_text.size()==0){
		QLabel* error_text = findChild<QLabel*>("intro_text");
		error_text->setText("Se deben rellenar todos los campos!");
		return;
	}
	try{
		client_session.emplace(ip_text.c_str(), puerto_text.c_str(), queue);
		client_session->start();
		this->hide();
		initialwindow.show();
		initialwindow.setSession(&client_session.value());
	} catch (...) {
        QLabel* error_text = findChild<QLabel*>("intro_text");
		error_text->setText("Error al intentar conectarse");
    }
}

void HostWindow::connectEvents() {
    QPushButton* bcontinue = findChild<QPushButton*>("bcontinue");
    QObject::connect(bcontinue, &QPushButton::clicked, this, &HostWindow::createConection);
}

ClientSession& HostWindow::getSession(){
	return client_session.value();
}

HostWindow::~HostWindow()
{
	delete ui;
}
