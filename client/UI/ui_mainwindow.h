/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_2;
    QPushButton *Van;
    QPushButton *Ferrari;
    QPushButton *Celeste;
    QPushButton *Jeep;
    QPushButton *Pickup;
    QPushButton *Limo;
    QPushButton *Descapotable;
    QLabel *intro_text;
    QPushButton *Ready;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1920, 1080);
        MainWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(440, 270, 1121, 241));
        label->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/Assets/Start.png")));
        label->setScaledContents(true);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(540, 410, 901, 161));
        label_2->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/Assets/Cars.png")));
        label_2->setScaledContents(true);
        Van = new QPushButton(centralwidget);
        Van->setObjectName(QString::fromUtf8("Van"));
        Van->setGeometry(QRect(550, 440, 111, 121));
        Van->setCursor(QCursor(Qt::PointingHandCursor));
        Van->setAutoFillBackground(false);
        Van->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Van->setFlat(true);
        Ferrari = new QPushButton(centralwidget);
        Ferrari->setObjectName(QString::fromUtf8("Ferrari"));
        Ferrari->setGeometry(QRect(670, 440, 111, 121));
        Ferrari->setCursor(QCursor(Qt::PointingHandCursor));
        Ferrari->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Ferrari->setFlat(true);
        Celeste = new QPushButton(centralwidget);
        Celeste->setObjectName(QString::fromUtf8("Celeste"));
        Celeste->setGeometry(QRect(790, 440, 111, 121));
        Celeste->setCursor(QCursor(Qt::PointingHandCursor));
        Celeste->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Celeste->setFlat(true);
        Jeep = new QPushButton(centralwidget);
        Jeep->setObjectName(QString::fromUtf8("Jeep"));
        Jeep->setGeometry(QRect(920, 440, 111, 121));
        Jeep->setCursor(QCursor(Qt::PointingHandCursor));
        Jeep->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Jeep->setFlat(true);
        Pickup = new QPushButton(centralwidget);
        Pickup->setObjectName(QString::fromUtf8("Pickup"));
        Pickup->setGeometry(QRect(1050, 440, 121, 121));
        Pickup->setCursor(QCursor(Qt::PointingHandCursor));
        Pickup->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Pickup->setFlat(true);
        Limo = new QPushButton(centralwidget);
        Limo->setObjectName(QString::fromUtf8("Limo"));
        Limo->setGeometry(QRect(1190, 420, 101, 141));
        Limo->setCursor(QCursor(Qt::PointingHandCursor));
        Limo->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Limo->setFlat(true);
        Descapotable = new QPushButton(centralwidget);
        Descapotable->setObjectName(QString::fromUtf8("Descapotable"));
        Descapotable->setGeometry(QRect(1300, 470, 121, 91));
        Descapotable->setCursor(QCursor(Qt::PointingHandCursor));
        Descapotable->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: none;       "));
        Descapotable->setFlat(true);
        intro_text = new QLabel(centralwidget);
        intro_text->setObjectName(QString::fromUtf8("intro_text"));
        intro_text->setGeometry(QRect(540, 620, 891, 131));
        QFont font;
        font.setFamily(QString::fromUtf8("Uroob"));
        font.setBold(false);
        font.setItalic(false);
        intro_text->setFont(font);
        intro_text->setStyleSheet(QString::fromUtf8("Color: white; \n"
"font-size: 36px; \n"
"\n"
"	      "));
        intro_text->setTextFormat(Qt::MarkdownText);
        intro_text->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        Ready = new QPushButton(centralwidget);
        Ready->setObjectName(QString::fromUtf8("Ready"));
        Ready->setGeometry(QRect(850, 580, 261, 41));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Ready->sizePolicy().hasHeightForWidth());
        Ready->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("System-ui"));
        font1.setPointSize(20);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setUnderline(false);
        font1.setStrikeOut(false);
        font1.setKerning(true);
        Ready->setFont(font1);
        Ready->setLayoutDirection(Qt::LeftToRight);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QString());
        label_2->setText(QString());
        Van->setText(QString());
        Ferrari->setText(QString());
        Celeste->setText(QString());
        Jeep->setText(QString());
        Pickup->setText(QString());
        Limo->setText(QString());
        Descapotable->setText(QString());
        intro_text->setText(QString());
        Ready->setText(QCoreApplication::translate("MainWindow", "START", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
