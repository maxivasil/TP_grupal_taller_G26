#include "toureditor.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <fstream>
#include <string>


TourEditor::TourEditor(QWidget* parent): QDialog(parent) {
    setWindowTitle("Editor de Tours");
    resize(500, 480);
    setWindowIcon(QIcon(ABS_DIR ASSETS_DIR "logo.png"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* tourBtnLayout = new QHBoxLayout;
    tour1Btn = new QPushButton("Tour 1");
    tour2Btn = new QPushButton("Tour 2");
    tour3Btn = new QPushButton("Tour 3");

    tourBtnLayout->addWidget(tour1Btn);
    tourBtnLayout->addWidget(tour2Btn);
    tourBtnLayout->addWidget(tour3Btn);

    mainLayout->addLayout(tourBtnLayout);

    connect(tour1Btn, &QPushButton::clicked, this, [this]() { loadTour(0); });
    connect(tour2Btn, &QPushButton::clicked, this, [this]() { loadTour(1); });
    connect(tour3Btn, &QPushButton::clicked, this, [this]() { loadTour(2); });

    mainLayout->addWidget(new QLabel("Carreras en este Tour:"));

    raceList = new QListWidget;
    raceList->setSelectionMode(QAbstractItemView::SingleSelection);
    raceList->setDragDropMode(QAbstractItemView::InternalMove);
    mainLayout->addWidget(raceList, 1);

    QHBoxLayout* orderLayout = new QHBoxLayout;
    upBtn = new QPushButton("Subir ↑");
    downBtn = new QPushButton("Bajar ↓");

    orderLayout->addWidget(upBtn);
    orderLayout->addWidget(downBtn);
    mainLayout->addLayout(orderLayout);

    connect(upBtn, &QPushButton::clicked, this, &TourEditor::moveUp);
    connect(downBtn, &QPushButton::clicked, this, &TourEditor::moveDown);

    removeBtn = new QPushButton("Quitar carrera del tour");
    mainLayout->addWidget(removeBtn);
    connect(removeBtn, &QPushButton::clicked, this, &TourEditor::removeRace);

    mainLayout->addWidget(new QLabel("Agregar carrera al tour:"));

    QHBoxLayout* addLayout = new QHBoxLayout;
    addRaceCombo = new QComboBox;
    addBtn = new QPushButton("Agregar");

    addLayout->addWidget(addRaceCombo);
    addLayout->addWidget(addBtn);
    mainLayout->addLayout(addLayout);

    connect(addBtn, &QPushButton::clicked, this, &TourEditor::addRaceToTour);

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addStretch();
    cancelBtn = new QPushButton("Cancelar");
    saveBtn = new QPushButton("Guardar");

    bottom->addWidget(cancelBtn);
    bottom->addWidget(saveBtn);
    mainLayout->addLayout(bottom);

    connect(cancelBtn, &QPushButton::clicked, this, &TourEditor::reject);
    connect(saveBtn, &QPushButton::clicked, this, &TourEditor::accept);

    loadTourDataFromFiles();
    loadTour(0);
}

void TourEditor::loadTourDataFromFiles() {
    for (int i = 0; i < 3; i++) {
        try {
            tourData[i] = YAML::LoadFile(tourFiles[i].toStdString());
        } catch (...) {
            qDebug() << "Error cargando" << tourFiles[i];
            tourData[i] = YAML::Node();
            tourData[i]["races"] = YAML::Node(YAML::NodeType::Sequence);
        }
    }
}

void TourEditor::loadTour(int index) {
    currentTour = index;
    raceList->clear();

    YAML::Node races = tourData[index]["races"];
    if (!races || !races.IsSequence())
        return;

    for (std::size_t i = 0; i < races.size(); ++i) {
        QString city = QString::fromStdString(races[i]["city"].as<std::string>());
        QString track = QString::fromStdString(races[i]["track"].as<std::string>());

        raceList->addItem(city + " - " + track);
    }
}

void TourEditor::moveUp() {
    int row = raceList->currentRow();
    if (row > 0) {
        QListWidgetItem* item = raceList->takeItem(row);
        raceList->insertItem(row - 1, item);
        raceList->setCurrentRow(row - 1);
    }
}

void TourEditor::moveDown() {
    int row = raceList->currentRow();
    if (row < raceList->count() - 1) {
        QListWidgetItem* item = raceList->takeItem(row);
        raceList->insertItem(row + 1, item);
        raceList->setCurrentRow(row + 1);
    }
}

void TourEditor::removeRace() { delete raceList->takeItem(raceList->currentRow()); }

void TourEditor::addRaceToTour() {
    QString race = addRaceCombo->currentText();
    raceList->addItem(race);
}

void TourEditor::setAvailableRaces(const QVector<QString>& list) {
    availableRaceItems = list;
    addRaceCombo->clear();

    for (const QString& item: list) {
        addRaceCombo->addItem(item);
    }
}

void TourEditor::accept() {
    YAML::Node newRaces(YAML::NodeType::Sequence);

    for (int i = 0; i < raceList->count(); i++) {
        QString line = raceList->item(i)->text();
        QStringList parts = line.split(" - ");
        if (parts.size() != 2)
            continue;

        YAML::Node entry;
        entry["city"] = parts[0].toStdString();
        entry["track"] = parts[1].toStdString();

        newRaces.push_back(entry);
    }

    YAML::Node tourNode;
    tourNode["races"] = newRaces;

    YAML::Emitter out;
    out << tourNode << YAML::Newline;

    std::ofstream fout(tourFiles[currentTour].toStdString());
    fout << out.c_str();
    fout.close();

    QDialog::accept();
}
