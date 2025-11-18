/********************************************************************************
** Form generated from reading UI file 'readywindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_READYWINDOW_H
#define UI_READYWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReadyWindow {
public:
    QWidget* centralwidget;
    QLabel* label_2;
    QLabel* intro_text;
    QLabel* Lobby_text;
    QPushButton* Ready;

    void setupUi(QMainWindow* ReadyWindow) {
        if (ReadyWindow->objectName().isEmpty())
            ReadyWindow->setObjectName(QString::fromUtf8("ReadyWindow"));
        ReadyWindow->resize(1920, 1080);
        ReadyWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(ReadyWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(530, 240, 901, 161));
        QFont font;
        font.setFamily(QString::fromUtf8("Uroob"));
        font.setPointSize(30);
        font.setBold(false);
        label_2->setFont(font);
        label_2->setTextFormat(Qt::AutoText);
        label_2->setScaledContents(true);
        label_2->setAlignment(Qt::AlignCenter);
        intro_text = new QLabel(centralwidget);
        intro_text->setObjectName(QString::fromUtf8("newText"));
        intro_text->setGeometry(QRect(550, 560, 891, 131));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Uroob"));
        font1.setBold(false);
        font1.setItalic(false);
        intro_text->setFont(font1);
        intro_text->setStyleSheet(QString::fromUtf8("Color: white; \n"
                                                    "font-size: 36px; \n"
                                                    "\n"
                                                    "	      "));
        intro_text->setTextFormat(Qt::AutoText);
        intro_text->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        Lobby_text = new QLabel(centralwidget);
        Lobby_text->setObjectName(QString::fromUtf8("Lobby_text"));
        Lobby_text->setGeometry(QRect(10, 10, 511, 141));
        Lobby_text->setFont(font1);
        Lobby_text->setStyleSheet(QString::fromUtf8("Color: white; \n"
                                                    "font-size: 65px; \n"
                                                    "\n"
                                                    "	      "));
        Lobby_text->setTextFormat(Qt::MarkdownText);
        Lobby_text->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
        Ready = new QPushButton(centralwidget);
        Ready->setObjectName(QString::fromUtf8("Ready"));
        Ready->setGeometry(QRect(810, 420, 341, 51));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Uroob"));
        font2.setPointSize(22);
        font2.setBold(false);
        font2.setItalic(false);
        Ready->setFont(font2);
        ReadyWindow->setCentralWidget(centralwidget);

        retranslateUi(ReadyWindow);

        QMetaObject::connectSlotsByName(ReadyWindow);
    }  // setupUi

    void retranslateUi(QMainWindow* ReadyWindow) {
        ReadyWindow->setWindowTitle(QCoreApplication::translate("NFS-2D", "NFS-2D", nullptr));
        label_2->setText(QCoreApplication::translate(
                "ReadyWindow",
                "<html><head/><body><p align=\"center\">Todo Listo! <br/>Recuerda esperar a todos "
                "los jugadores antes de empezar la partida.</p></body></html>",
                nullptr));
        intro_text->setText(QString());
        Lobby_text->setText(QCoreApplication::translate("ReadyWindow", "asdasd", nullptr));
        Ready->setText(QCoreApplication::translate("ReadyWindow", "Listo para empezar!", nullptr));
    }  // retranslateUi
};

namespace Ui {
class ReadyWindow: public Ui_ReadyWindow {};
}  // namespace Ui

QT_END_NAMESPACE

#endif  // UI_READYWINDOW_H
