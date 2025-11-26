#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>

#include <yaml-cpp/yaml.h>

#include "checkpoint.h"
#include "mapview.h"
#include "toureditor.h"

class QListWidget;
class QPushButton;
class QLabel;

class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // cppcheck-suppress unknownMacro
private slots:
    void openMap(const QString& path, const QString& displayName);
    void onAddCheckpoint();
    void onRectCreated(const QRectF& rect);
    void onExportYaml();
    void onRemoveSelected();
    void updateSidebarTitle();

private:
    QWidget* central;
    MapView* mapView;
    QListWidget* checkpointList;
    QPushButton* addBtn;
    QPushButton* exportBtn;
    QPushButton* removeBtn;
    QLabel* sidebarTitle;
    QPushButton* editToursBtn;

    QString currentMapName;
    QString currentMapPath;
    QList<Checkpoint> checkpoints;

    QPushButton* map1;
    QPushButton* map2;
    QPushButton* map3;

    QPointF initialDirection;
    QPushButton* directionBtn;

    void refreshCheckpointList();
};

#endif  // MAINWINDOW_H
