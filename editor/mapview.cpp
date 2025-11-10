#include "mapview.h"

#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QPixmap>
#include <QScrollBar>
#include <QWheelEvent>
#include <cmath>

MapView::MapView(QWidget* parent):
        QGraphicsView(parent),
        scene(new QGraphicsScene(this)),
        pixmapItem(nullptr),
        drawingMode(false),
        dragging(false),
        rubberBandItem(nullptr),
        currentScale(1.0),
        directionMode(false) {
    setScene(scene);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::NoAnchor);
}

bool MapView::loadImage(const QString& path) {
    QPixmap px(path);
    if (px.isNull()) {
        return false;
    }
    scene->clear();
    pixmapItem = scene->addPixmap(px);
    pixmapItem->setPos(0, 0);

    scene->setSceneRect(0, 0, px.width(), px.height());
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    currentScale = transform().m11();
    return true;
}

void MapView::setDrawingMode(bool on) {
    drawingMode = on;
    if (on) {
        setDragMode(QGraphicsView::NoDrag);
        setInteractive(false);
    } else {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setInteractive(true);
        if (rubberBandItem) {
            scene->removeItem(rubberBandItem);
            delete rubberBandItem;
            rubberBandItem = nullptr;
        }
    }
}

void MapView::wheelEvent(QWheelEvent* event) {
    const QPoint numDegrees = event->angleDelta();

    if (event->modifiers() & Qt::ControlModifier) {  // === Zoom con Ctrl ===
        const double zoomFactor = 1.15;
        double factor = (numDegrees.y() > 0) ? zoomFactor : 1.0 / zoomFactor;
        QPointF scenePos = mapToScene(event->position().toPoint());
        scale(factor, factor);
        QPointF newScenePos = mapToScene(event->position().toPoint());
        QPointF delta = newScenePos - scenePos;
        translate(delta.x(), delta.y());
    } else if (event->modifiers() & Qt::ShiftModifier) {  // === Scroll horizontal con Shift ===
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - numDegrees.y());
    } else {  // === Scroll vertical normal ===
        verticalScrollBar()->setValue(verticalScrollBar()->value() - numDegrees.y());
    }

    event->accept();
}

void MapView::zoomIn() {
    const double scaleFactor = 1.15;
    scale(scaleFactor, scaleFactor);
    currentScale *= scaleFactor;
}

void MapView::zoomOut() {
    const double scaleFactor = 1.15;
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    currentScale /= scaleFactor;
}

void MapView::resetZoom() {
    resetTransform();
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    currentScale = transform().m11();
}

void MapView::mousePressEvent(QMouseEvent* event) {
    if (directionMode && event->button() == Qt::LeftButton) {
        initialDirStart = mapToScene(event->pos());
        event->accept();
        return;
    }
    if (drawingMode && event->button() == Qt::LeftButton) {
        dragging = true;
        startPos = event->pos();

        QPointF s = mapToScene(startPos);
        if (!rubberBandItem) {
            rubberBandItem =
                    scene->addRect(QRectF(s, s), QPen(Qt::red), QBrush(QColor(255, 0, 0, 50)));
        } else {
            rubberBandItem->setRect(QRectF(s, s));
        }
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseMoveEvent(QMouseEvent* event) {
    if (drawingMode && dragging) {
        QPoint cur = event->pos();
        QPointF s = mapToScene(startPos);
        QPointF c = mapToScene(cur);
        QRectF rect(s, c);
        rect = rect.normalized();
        if (!rubberBandItem) {
            rubberBandItem = scene->addRect(rect, QPen(Qt::red), QBrush(QColor(255, 0, 0, 50)));
        } else {
            rubberBandItem->setRect(rect);
        }
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent* event) {
    if (drawingMode && dragging && event->button() == Qt::LeftButton) {
        dragging = false;
        QPointF s = mapToScene(startPos);
        QPointF c = mapToScene(event->pos());
        QRectF rect(s, c);
        rect = rect.normalized();

        if (scene->sceneRect().intersects(rect)) {
            QRectF intersected = rect.intersected(scene->sceneRect());
            emit rectCreated(intersected);
            checkpoints.push_back(intersected);
            emit checkpointsChanged(checkpoints.size());
        } else {
        }

        if (rubberBandItem) {
            scene->removeItem(rubberBandItem);
            delete rubberBandItem;
            rubberBandItem = nullptr;
        }

        setDrawingMode(false);
        event->accept();
        return;
    }
    if (directionMode && event->button() == Qt::LeftButton) {
        initialDirEnd = mapToScene(event->pos());
        QPointF dir = initialDirEnd - initialDirStart;
        if (!dir.isNull()) {
            double len = std::hypot(dir.x(), dir.y());
            QPointF normalized(dir.x() / len, dir.y() / len);
            emit initialDirectionDefined(normalized);
        }
        setDirectionMode(false);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapView::clearCheckpoints() {
    for (auto* item: checkpointItems) {
        if (item && scene) {
            scene->removeItem(item);
        }
        delete item;
    }
    checkpointItems.clear();
    emit checkpointsChanged(0);
}

void MapView::setDirectionMode(bool on) {
    directionMode = on;
    if (on) {
        setDragMode(QGraphicsView::NoDrag);
        setInteractive(false);
    } else {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setInteractive(true);
    }
}

void MapView::clearInitialDirection() {
    initialDirStart = QPointF();
    initialDirEnd = QPointF();
}

void MapView::showCheckpoints(const QList<Checkpoint>& checkpoints) {
    for (auto* item: checkpointItems) {
        scene->removeItem(item);
        delete item;
    }
    checkpointItems.clear();

    for (int i = 0; i < checkpoints.size(); ++i) {
        const Checkpoint& cp = checkpoints[i];
        QRectF rect = cp.rect;

        auto* rectItem = scene->addRect(rect, QPen(Qt::green, 2), QBrush(QColor(0, 255, 0, 80)));
        checkpointItems.append(rectItem);

        auto* label = scene->addText(QString::number(i + 1));
        label->setDefaultTextColor(Qt::white);
        label->setPos(rect.center().x() - 5, rect.center().y() - 10);
        checkpointItems.append(label);
    }
}

void MapView::clearScene() { scene->clear(); }
