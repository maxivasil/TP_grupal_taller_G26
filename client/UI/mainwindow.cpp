#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
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


void MainWindow::selector(){
	QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
	QLabel* labelOut = findChild<QLabel*>("intro_text");
	QString carName = senderButton->property("car_name").toString();
	labelOut->setText("Has seleccionado un " + carName + "\n\n Esperando al resto de jugadores...");
}

void MainWindow::connectEvents() {
	QPushButton* celeste = findChild<QPushButton*>("Celeste");
	celeste->setProperty("car_name", "Chevrolet Corsa");
	QObject::connect(celeste, &QPushButton::clicked,
			 this, &MainWindow::selector);

	QPushButton* descapotable = findChild<QPushButton*>("Descapotable");
	descapotable->setProperty("car_name", "Lamborghini Diablo");
	QObject::connect(descapotable, &QPushButton::clicked,
			 this, &MainWindow::selector);

	QPushButton* ferrari = findChild<QPushButton*>("Ferrari");
	ferrari->setProperty("car_name", "Dodge Viper");
	QObject::connect(ferrari, &QPushButton::clicked,
			 this, &MainWindow::selector);

	QPushButton* jeep = findChild<QPushButton*>("Jeep");
	jeep->setProperty("car_name", "Jeep Wrangler");
	QObject::connect(jeep, &QPushButton::clicked,
			 this, &MainWindow::selector);

	QPushButton* limo = findChild<QPushButton*>("Limo");
	limo->setProperty("car_name", "Lincoln TownCar");
	QObject::connect(limo, &QPushButton::clicked,
			 this, &MainWindow::selector);

	QPushButton* pickup = findChild<QPushButton*>("Pickup");
	pickup->setProperty("car_name", "Toyota Tacoma");
	QObject::connect(pickup, &QPushButton::clicked,
			 this, &MainWindow::selector);

	QPushButton* van = findChild<QPushButton*>("Van");
	van->setProperty("car_name", "Iveco Daily");
	QObject::connect(van, &QPushButton::clicked,
			 this, &MainWindow::selector);
}

MainWindow::~MainWindow()
{
	delete ui;
}
