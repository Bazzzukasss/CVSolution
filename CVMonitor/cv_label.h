#ifndef CVLABEL_H
#define CVLABEL_H

#include <QLabel>
#include <QPixmap>
#include "coco_item.h"

class CVLabel : public QLabel
{
public:
    CVLabel(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);
    void setReferenceAnnotations(const CocoRootItem &item);
    void setEvaluatedAnnotations(const CocoRootItem &item);

private:
    PaintData mPaintData;
    QPixmap mPixmap;
    CocoRootItem mReferenceCocoItem;
    CocoRootItem mEvaluatedCocoItem;

    void paintImage(QPainter& painter);
    void paintAnnotations(QPainter& painter);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CVLABEL_H
