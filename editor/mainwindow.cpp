#include "mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTextStream>
#include <QVBoxLayout>

#include "../common/constants.h"

MainWindow::MainWindow(QWidget* parent):
        QMainWindow(parent), central(new QWidget(this)), mapView(new MapView(this)) {
    setWindowTitle("Editor de Carreras");
    setMinimumSize(1000, 600);

    // Left: mapView
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // Controls: open map buttons for 3 images
    QHBoxLayout* mapBtnsLayout = new QHBoxLayout;
    map1 = new QPushButton("Liberty City");
    map2 = new QPushButton("San Andreas");
    map3 = new QPushButton("Vice City");
    mapBtnsLayout->addWidget(map1);
    mapBtnsLayout->addWidget(map2);
    mapBtnsLayout->addWidget(map3);
    mapBtnsLayout->addStretch();

    leftLayout->addLayout(mapBtnsLayout);
    leftLayout->addWidget(mapView, 1);

    // Zoom controls
    // cppcheck-suppress constVariablePointer
    QHBoxLayout* zoomLayout = new QHBoxLayout;
    // cppcheck-suppress constVariablePointer
    QPushButton* zoomIn = new QPushButton("+");
    // cppcheck-suppress constVariablePointer
    QPushButton* zoomOut = new QPushButton("-");
    // cppcheck-suppress constVariablePointer
    QPushButton* resetZoom = new QPushButton("100%");
    zoomLayout->addWidget(new QLabel("Zoom:"));
    zoomLayout->addWidget(zoomIn);
    zoomLayout->addWidget(zoomOut);
    zoomLayout->addWidget(resetZoom);
    zoomLayout->addStretch();
    leftLayout->addLayout(zoomLayout);

    // Right: sidebar
    QWidget* rightPanel = new QWidget;
    rightPanel->setFixedWidth(320);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    directionBtn = new QPushButton("Definir dirección inicial");
    rightLayout->addWidget(directionBtn);

    sidebarTitle = new QLabel("Checkpoints para <ninguno>");
    sidebarTitle->setWordWrap(true);
    sidebarTitle->setStyleSheet("font-weight: bold; font-size: 16px;");
    rightLayout->addWidget(sidebarTitle);

    checkpointList = new QListWidget;
    checkpointList->setSelectionMode(QAbstractItemView::SingleSelection);
    checkpointList->setDragDropMode(QAbstractItemView::InternalMove);
    rightLayout->addWidget(checkpointList, 1);

    addBtn = new QPushButton("Agregar checkpoint");
    removeBtn = new QPushButton("Eliminar seleccionado");
    exportBtn = new QPushButton("Exportar YAML");

    rightLayout->addWidget(addBtn);
    rightLayout->addWidget(removeBtn);
    rightLayout->addStretch();
    rightLayout->addWidget(exportBtn);

    // Main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel);

    setCentralWidget(central);

    // Connections
    connect(map1, &QPushButton::clicked, this, [this]() {
        openMap(ABS_DIR ASSETS_DIR "cities/"
                                   "Liberty_City.png",
                "Liberty City");
    });
    connect(map2, &QPushButton::clicked, this, [this]() {
        openMap(ABS_DIR ASSETS_DIR "cities/"
                                   "San_Andreas.png",
                "San Andreas");
    });
    connect(map3, &QPushButton::clicked, this, [this]() {
        openMap(ABS_DIR ASSETS_DIR "cities/"
                                   "Vice_City.png",
                "Vice City");
    });

    connect(zoomIn, &QPushButton::clicked, mapView, &MapView::zoomIn);
    connect(zoomOut, &QPushButton::clicked, mapView, &MapView::zoomOut);
    connect(resetZoom, &QPushButton::clicked, mapView, &MapView::resetZoom);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddCheckpoint);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportYaml);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveSelected);

    connect(mapView, &MapView::rectCreated, this, &MainWindow::onRectCreated);

    connect(checkpointList->model(), &QAbstractItemModel::rowsMoved, this, [this]() {
        QList<Checkpoint> newList;
        for (int i = 0; i < checkpointList->count(); ++i) {
            QVariant var = checkpointList->item(i)->data(Qt::UserRole);
            Checkpoint cp = checkpointList->item(i)->data(Qt::UserRole + 1).value<Checkpoint>();
            newList.append(cp);
        }
        checkpoints = newList;
    });

    connect(mapView, &MapView::checkpointsChanged, this, [this](int count) {
        bool hasCheckpoints = (count > 0);
        map1->setEnabled(!hasCheckpoints);
        map2->setEnabled(!hasCheckpoints);
        map3->setEnabled(!hasCheckpoints);
    });

    connect(directionBtn, &QPushButton::clicked, this, [this]() {
        mapView->setDirectionMode(true);
        setCursor(Qt::CrossCursor);
    });

    connect(mapView, &MapView::initialDirectionDefined, this, [this](QPointF dir) {
        setCursor(Qt::ArrowCursor);
        initialDirection = dir;
        QMessageBox::information(this, "Dirección definida",
                                 QString("Dirección inicial: (x=%1, y=%2)")
                                         .arg(dir.x(), 0, 'f', 2)
                                         .arg(dir.y(), 0, 'f', 2));
    });
}

MainWindow::~MainWindow() {}

void MainWindow::openMap(const QString& path, const QString& displayName) {
    if (!mapView->loadImage(path)) {
        QMessageBox::warning(this, "Error", "No se pudo cargar la imagen: " + path);
        return;
    }
    currentMapName = displayName;
    currentMapPath = path;
    checkpoints.clear();
    mapView->clearCheckpoints();
    refreshCheckpointList();
    updateSidebarTitle();
    mapView->showCheckpoints(checkpoints);
}

void MainWindow::onAddCheckpoint() {
    mapView->setDrawingMode(true);
    setCursor(Qt::CrossCursor);
}

void MainWindow::onRectCreated(const QRectF& rect) {
    setCursor(Qt::ArrowCursor);
    mapView->setDrawingMode(false);

    Checkpoint cp;
    cp.name = QString("CP%1").arg(checkpoints.size() + 1);
    cp.rect = rect;
    checkpoints.append(cp);
    refreshCheckpointList();

    mapView->showCheckpoints(checkpoints);
}

void MainWindow::refreshCheckpointList() {
    checkpointList->clear();
    for (int i = 0; i < checkpoints.size(); ++i) {
        const Checkpoint& cp = checkpoints.at(i);
        QString label = QString("%1: (cx=%2, cy=%3, w=%4, h=%5)")
                                .arg(cp.name)
                                .arg(int(cp.rect.center().x()))
                                .arg(int(cp.rect.center().y()))
                                .arg(int(cp.rect.width()))
                                .arg(int(cp.rect.height()));
        QListWidgetItem* item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, i);

        QVariant v;
        v.setValue(cp);
        item->setData(Qt::UserRole + 1, QVariant::fromValue(cp));
        checkpointList->addItem(item);
    }
    updateSidebarTitle();
}

void MainWindow::onExportYaml() {
    if (currentMapPath.isEmpty()) {
        QMessageBox::information(this, "Info", "Abre un mapa primero.");
        return;
    }

    if (initialDirection.isNull()) {
        QMessageBox::warning(this, "Falta dirección inicial",
                             "Debe definir la dirección inicial antes de exportar.");
        return;
    }

    if (checkpointList->count() == 0) {
        QMessageBox::warning(this, "Falta checkpoints",
                             "Debe definir algún checkpoint antes de exportar.");
        return;
    }

    QString filename =
            QFileDialog::getSaveFileName(this, "Guardar YAML", QString(), "YAML (*.yaml *.yml)");
    if (filename.isEmpty()) {
        return;
    }

    QList<Checkpoint> ordered;
    for (int i = 0; i < checkpointList->count(); ++i) {
        Checkpoint cp = checkpointList->item(i)->data(Qt::UserRole + 1).value<Checkpoint>();
        ordered.append(cp);
    }

    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo crear archivo.");
        return;
    }

    QTextStream out(&f);
    if (!initialDirection.isNull()) {
        out << "initial_direction:\n";
        out << "  x: " << QString::number(initialDirection.x(), 'f', 3) << "\n";
        out << "  y: " << QString::number(initialDirection.y(), 'f', 3) << "\n\n";
    }

    double scaleX = 0.14325000;
    double scaleY = 0.12858333;
    out << "checkpoints:\n";
    for (const Checkpoint& cp: ordered) {
        double cx = cp.rect.center().x() * scaleX;
        double cy = cp.rect.center().y() * scaleY;
        double w = cp.rect.width() * scaleX;
        double h = cp.rect.height() * scaleY;
        out << "  - x: " << QString::number(cx, 'f', 1) << "\n";
        out << "    y: " << QString::number(cy, 'f', 1) << "\n";
        out << "    width: " << QString::number(w, 'f', 1) << "\n";
        out << "    height: " << QString::number(h, 'f', 1) << "\n";
    }

    f.close();
    QMessageBox::information(this, "Exportado", "YAML guardado en:\n" + filename);

    mapView->clearCheckpoints();
    mapView->clearInitialDirection();
    checkpointList->clear();
    checkpoints.clear();
    checkpoints.clear();
    currentMapName.clear();
    currentMapPath.clear();
    updateSidebarTitle();
    map1->setEnabled(true);
    map2->setEnabled(true);
    map3->setEnabled(true);
    mapView->clearScene();
}

void MainWindow::onRemoveSelected() {
    int row = checkpointList->currentRow();
    if (row < 0) {
        return;
    }
    checkpoints.removeAt(row);
    refreshCheckpointList();
    mapView->showCheckpoints(checkpoints);
    emit mapView->checkpointsChanged(checkpoints.size());
}

void MainWindow::updateSidebarTitle() {
    if (currentMapName.isEmpty()) {
        sidebarTitle->setText("Checkpoints para <ninguno>");
    } else {
        sidebarTitle->setText(QString("Checkpoints para %1").arg(currentMapName));
    }
}
