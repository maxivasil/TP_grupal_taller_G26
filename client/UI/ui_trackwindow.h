/********************************************************************************
** Form generated from reading UI file 'trackwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKWINDOW_H
#define UI_TRACKWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackWindow {
public:
    QWidget* centralwidget;
    QLabel* intro_text_2;
    QPushButton* intermediate;
    QPushButton* begginer;
    QPushButton* expert;

    void setupUi(QMainWindow* TrackWindow) {
        if (TrackWindow->objectName().isEmpty())
            TrackWindow->setObjectName(QString::fromUtf8("TrackWindow"));
        TrackWindow->resize(1920, 1080);
        TrackWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(TrackWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        intro_text_2 = new QLabel(centralwidget);
        intro_text_2->setObjectName(QString::fromUtf8("intro_text_2"));
        intro_text_2->setGeometry(QRect(430, 220, 1171, 101));
        QFont font;
        font.setFamily(QString::fromUtf8("Uroob"));
        font.setBold(false);
        font.setItalic(false);
        intro_text_2->setFont(font);
        intro_text_2->setStyleSheet(QString::fromUtf8("Color: white; \n"
                                                      "font-size: 70px; \n"
                                                      "\n"
                                                      "	      "));
        intro_text_2->setLocale(QLocale(QLocale::Chuvash, QLocale::Russia));
        intro_text_2->setTextFormat(Qt::MarkdownText);
        intro_text_2->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        intermediate = new QPushButton(centralwidget);
        intermediate->setCursor(QCursor(Qt::PointingHandCursor));
        intermediate->setObjectName(QString::fromUtf8("intermediate"));
        intermediate->setGeometry(QRect(760, 730, 451, 261));
        intermediate->setStyleSheet(QString::fromUtf8("background: transparent;\n"
                                                      "border: none;       "));
        begginer = new QPushButton(centralwidget);
        begginer->setCursor(QCursor(Qt::PointingHandCursor));
        begginer->setObjectName(QString::fromUtf8("begginer"));
        begginer->setGeometry(QRect(240, 730, 451, 261));
        begginer->setStyleSheet(QString::fromUtf8("background: transparent;\n"
                                                  "border: none;       "));
        expert = new QPushButton(centralwidget);
        expert->setCursor(QCursor(Qt::PointingHandCursor));
        expert->setObjectName(QString::fromUtf8("expert"));
        expert->setGeometry(QRect(1280, 730, 451, 261));
        expert->setStyleSheet(QString::fromUtf8("background: transparent;\n"
                                                "border: none;       "));
        TrackWindow->setCentralWidget(centralwidget);

        retranslateUi(TrackWindow);

        QMetaObject::connectSlotsByName(TrackWindow);
    }  // setupUi

    void retranslateUi(QMainWindow* TrackWindow) {
        TrackWindow->setWindowTitle(
                QCoreApplication::translate("TrackWindow", "InitialWindow", nullptr));
        intro_text_2->setText(QCoreApplication::translate(
                "TrackWindow", "Seleccione sobre que Pista se jugar\303\241 la carrera", nullptr));
        expert->setText(QString());
        begginer->setText(QString());
        intermediate->setText(QString());
    }  // retranslateUi
};

namespace Ui {
class TrackWindow: public Ui_TrackWindow {};
}  // namespace Ui

QT_END_NAMESPACE

#endif  // UI_TRACKWINDOW_H
