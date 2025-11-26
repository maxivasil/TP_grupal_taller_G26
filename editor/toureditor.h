#ifndef TOUREDITOR_H
#define TOUREDITOR_H

#include <QComboBox>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>

#include <yaml-cpp/yaml.h>

#include "../common/constants.h"

class TourEditor: public QDialog {
    Q_OBJECT

public:
    explicit TourEditor(QWidget* parent = nullptr);
    void setAvailableRaces(const QVector<QString>& list);
    void loadTourDataFromFiles();

private slots:
    void loadTour(int index);
    void moveUp();
    void moveDown();
    void removeRace();
    void addRaceToTour();
    void accept() override;

private:
    int currentTour = 0;

    QPushButton* tour1Btn;
    QPushButton* tour2Btn;
    QPushButton* tour3Btn;

    QListWidget* raceList;
    QPushButton* upBtn;
    QPushButton* downBtn;
    QPushButton* removeBtn;

    QComboBox* addRaceCombo;
    QPushButton* addBtn;

    QPushButton* saveBtn;
    QPushButton* cancelBtn;

    QString tourFiles[3] = {ABS_DIR "tours/tour1.yaml", ABS_DIR "tours/tour2.yaml",
                            ABS_DIR "tours/tour3.yaml"};

    YAML::Node tourData[3];
    QVector<QString> allRacesList;
    QVector<QString> availableRaceItems;
};

#endif  // TOUREDITOR_H
