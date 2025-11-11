#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPointF>

#include "checkpoint.h"

class MapView: public QGraphicsView {
    Q_OBJECT
public:
    explicit MapView(QWidget* parent = nullptr);

    bool loadImage(const QString& path);
    void setDrawingMode(bool on);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void clearCheckpoints();
    void setDirectionMode(bool on);
    void clearInitialDirection();
    void showCheckpoints(const QList<Checkpoint>& checkpoints);
    void clearScene();

signals:
    void rectCreated(const QRectF& rect);
    void checkpointsChanged(int count);
    void initialDirectionDefined(QPointF dir);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem;
    bool drawingMode;
    bool dragging;
    QPoint startPos;
    QGraphicsRectItem* rubberBandItem;
    double currentScale;
    QVector<QRectF> checkpoints;
    void ensureSceneMatchesPixmap();
    bool directionMode = false;
    QPointF initialDirStart, initialDirEnd;
    QList<QGraphicsItem*> checkpointItems;
};

#endif  // MAPVIEW_H
