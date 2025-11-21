/********************************************************************************
** Form generated from reading UI file 'hostwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOSTWINDOW_H
#define UI_HOSTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HostWindow {
public:
    QWidget* centralwidget;
    QLabel* intro_text;
    QTextEdit* Username;
    QLabel* label_3;
    QTextEdit* IP;
    QTextEdit* Puerto;
    QLabel* label_4;
    QLabel* label_5;
    QPushButton* bcontinue;

    void setupUi(QMainWindow* HostWindow) {
        if (HostWindow->objectName().isEmpty())
            HostWindow->setObjectName(QString::fromUtf8("HostWindow"));
        HostWindow->resize(1920, 1080);
        HostWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(HostWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        QFont font;
        font.setFamily(QString::fromUtf8("Uroob"));
        font.setPointSize(30);
        font.setBold(false);
        bcontinue = new QPushButton(centralwidget);
        bcontinue->setObjectName(QString::fromUtf8("bcontinue"));
        bcontinue->setGeometry(QRect(860, 590, 281, 41));
        intro_text = new QLabel(centralwidget);
        intro_text->setObjectName(QString::fromUtf8("intro_text"));
        intro_text->setGeometry(QRect(520, 215, 901, 161));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Uroob"));
        font1.setBold(false);
        font1.setItalic(false);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Uroob"));
        font2.setBold(false);
        font2.setItalic(false);
        font2.setPointSize(22);
        bcontinue->setFont(font2);
        intro_text->setFont(font1);
        intro_text->setStyleSheet(QString::fromUtf8("Color: white; \n"
                                                    "font-size: 36px; \n"
                                                    "\n"
                                                    "	      "));
        intro_text->setTextFormat(Qt::MarkdownText);
        intro_text->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        Username = new QTextEdit(centralwidget);
        Username->setObjectName(QString::fromUtf8("Username"));
        Username->setGeometry(QRect(860, 340, 351, 51));
        Username->setStyleSheet("font-size: 30px;");
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(690, 345, 171, 41));
        label_3->setFont(font);
        label_3->setTextFormat(Qt::AutoText);
        label_3->setScaledContents(true);
        label_3->setAlignment(Qt::AlignCenter);
        IP = new QTextEdit(centralwidget);
        IP->setObjectName(QString::fromUtf8("IP"));
        IP->setGeometry(QRect(860, 410, 351, 51));
        IP->setStyleSheet("font-size: 30px;");
        Puerto = new QTextEdit(centralwidget);
        Puerto->setObjectName(QString::fromUtf8("Puerto"));
        Puerto->setGeometry(QRect(860, 490, 351, 51));
        Puerto->setStyleSheet("font-size: 30px;");
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(810, 415, 51, 41));
        label_4->setFont(font);
        label_4->setTextFormat(Qt::AutoText);
        label_4->setScaledContents(true);
        label_4->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(740, 495, 121, 41));
        label_5->setFont(font);
        label_5->setTextFormat(Qt::AutoText);
        label_5->setScaledContents(true);
        label_5->setAlignment(Qt::AlignCenter);
        HostWindow->setCentralWidget(centralwidget);

        retranslateUi(HostWindow);

        QMetaObject::connectSlotsByName(HostWindow);
    }  // setupUi

    void retranslateUi(QMainWindow* HostWindow) {
        HostWindow->setWindowTitle(QCoreApplication::translate("NFS-2D", "NFS-2D", nullptr));
        intro_text->setText(QString());
        bcontinue->setText(QCoreApplication::translate("HostWindow", "Continuar", nullptr));
        label_3->setText(
                QCoreApplication::translate("HostWindow",
                                            "<html><head/><body><p align=\"center\"><span style=\" "
                                            "font-size:36pt;\">Username:</span></p></body></html>",
                                            nullptr));
        label_4->setText(
                QCoreApplication::translate("HostWindow",
                                            "<html><head/><body><p align=\"center\"><span style=\" "
                                            "font-size:36pt;\">IP:</span></p></body></html>",
                                            nullptr));
        label_5->setText(
                QCoreApplication::translate("HostWindow",
                                            "<html><head/><body><p align=\"center\"><span style=\" "
                                            "font-size:36pt;\">Puerto:</span></p></body></html>",
                                            nullptr));
    }  // retranslateUi
};

namespace Ui {
class HostWindow: public Ui_HostWindow {};
}  // namespace Ui

QT_END_NAMESPACE

#endif  // UI_HOSTWINDOW_H
