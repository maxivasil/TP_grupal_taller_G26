#include "initialwindow.h"
#include "./ui_initialwindow.h"
#include <string>
#include <QPixmap>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include <QScreen>
#include <QGuiApplication>

InitialWindow::InitialWindow(QWidget *parent)
	: QMainWindow(parent)
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

void InitialWindow::joinlobby(){
	QLineEdit* lobby_line = findChild<QLineEdit*>("lobby");
	std::string lobby = (lobby_line->text()).toStdString();
	if(lobby.size() != 6){
		QLabel* labelOut = findChild<QLabel*>("intro_text");
		labelOut->setText("El Lobby debe tener exactamente 6 caracteres");
	}
}

void InitialWindow::connectEvents() {
	QPushButton* lobby = findChild<QPushButton*>("joinlobby");
	QObject::connect(lobby, &QPushButton::clicked,
			 this, &InitialWindow::joinlobby);
}

InitialWindow::~InitialWindow()
{
	delete ui;
}
