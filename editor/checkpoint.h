#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <QRectF>
#include <QString>

struct Checkpoint {
    QString name;
    QRectF rect;
};

Q_DECLARE_METATYPE(Checkpoint)

#endif  // CHECKPOINT_H
