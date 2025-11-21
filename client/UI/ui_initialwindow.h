/********************************************************************************
** Form generated from reading UI file 'initialwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INITIALWINDOW_H
#define UI_INITIALWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InitialWindow {
public:
    QWidget* centralwidget;
    QLabel* intro_text;
    QPushButton* newlobby;
    QLineEdit* lobby;
    QLabel* intro_text_2;
    QPushButton* joinlobby;

    void setupUi(QMainWindow* InitialWindow) {
        if (InitialWindow->objectName().isEmpty())
            InitialWindow->setObjectName(QString::fromUtf8("InitialWindow"));
        InitialWindow->resize(1920, 1080);
        InitialWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(InitialWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        intro_text = new QLabel(centralwidget);
        intro_text->setObjectName(QString::fromUtf8("intro_text"));
        intro_text->setGeometry(QRect(540, 640, 891, 131));
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
        intro_text->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        newlobby = new QPushButton(centralwidget);
        newlobby->setObjectName(QString::fromUtf8("newlobby"));
        newlobby->setGeometry(QRect(850, 580, 261, 41));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(newlobby->sizePolicy().hasHeightForWidth());
        newlobby->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("System-ui"));
        font1.setPointSize(14);
        font1.setBold(true);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setStrikeOut(false);
        font1.setKerning(true);
        newlobby->setFont(font1);
        newlobby->setLayoutDirection(Qt::LeftToRight);
        lobby = new QLineEdit(centralwidget);
        lobby->setObjectName(QString::fromUtf8("lobby"));
        lobby->setGeometry(QRect(830, 470, 251, 41));
        QFont font2;
        font2.setPointSize(17);
        lobby->setFont(font2);
        intro_text_2 = new QLabel(centralwidget);
        intro_text_2->setObjectName(QString::fromUtf8("intro_text_2"));
        intro_text_2->setGeometry(QRect(700, 400, 551, 61));
        QFont font3;
        font3.setFamily(QString::fromUtf8("System-ui"));
        font3.setBold(true);
        font3.setItalic(false);
        intro_text_2->setFont(font3);
        intro_text_2->setStyleSheet(QString::fromUtf8("Color: white; \n"
                                                      "font-size: 36px; \n"
                                                      "\n"
                                                      "	      "));
        intro_text_2->setTextFormat(Qt::MarkdownText);
        intro_text_2->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        joinlobby = new QPushButton(centralwidget);
        joinlobby->setObjectName(QString::fromUtf8("joinlobby"));
        joinlobby->setGeometry(QRect(1080, 470, 61, 41));
        QFont font4;
        font4.setPointSize(24);
        joinlobby->setFont(font4);
        InitialWindow->setCentralWidget(centralwidget);

        retranslateUi(InitialWindow);

        QMetaObject::connectSlotsByName(InitialWindow);
    }  // setupUi

    void retranslateUi(QMainWindow* InitialWindow) {
        InitialWindow->setWindowTitle(QCoreApplication::translate("NFS-2D", "NFS-2D", nullptr));
        intro_text->setText(QString());
        newlobby->setText(
                QCoreApplication::translate("InitialWindow", "Crear nueva partida", nullptr));
        lobby->setText(QString());
        intro_text_2->setText(QCoreApplication::translate(
                "InitialWindow", "Unirse a una partida Existente", nullptr));
        joinlobby->setText(QCoreApplication::translate("InitialWindow", "\342\256\225", nullptr));
    }  // retranslateUi
};

namespace Ui {
class InitialWindow: public Ui_InitialWindow {};
}  // namespace Ui

QT_END_NAMESPACE

#endif  // UI_INITIALWINDOW_H
