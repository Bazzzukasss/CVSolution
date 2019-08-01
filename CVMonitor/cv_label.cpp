#include "cv_label.h"
#include <QPainter>
#include "app_constants.h"

CVLabel::CVLabel(QWidget *parent)
    : QLabel(parent)
{    
}

void CVLabel::setPixmap(const QPixmap &pixmap)
{
    mPixmap = pixmap;

    repaint();
}

void CVLabel::setReferenceAnnotations(const CocoRootItem &item)
{
    mReferenceCocoItem = item;

    repaint();
}

void CVLabel::setEvaluatedAnnotations(const CocoRootItem &item)
{
    mEvaluatedCocoItem = item;

    repaint();
}

void CVLabel::paintImage(QPainter &painter)
{
    if(!mPixmap.isNull())
    {
        const auto& scaledPixmap = mPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);
        mPaintData.rect = QRect({(width() - scaledPixmap.width()) / 2, (height() - scaledPixmap.height()) / 2}, scaledPixmap.size());
        mPaintData.scale = static_cast<double>(scaledPixmap.width()) / static_cast<double>(mPixmap.width());

        painter.drawPixmap( mPaintData.rect, scaledPixmap );
    }
    else
    {
        mPaintData.rect = rect();
        mPaintData.scale = 1.0;
    }
}

void CVLabel::paintAnnotations(QPainter &painter)
{
    painter.setPen(app_config::kRefPenColor);
    painter.setBrush(app_config::kRefBrushColor);
    mReferenceCocoItem.paint(painter, mPaintData);

    painter.setPen(app_config::kEvalPenColor);
    painter.setBrush(app_config::kEvalBrushColor);
    mEvaluatedCocoItem.paint(painter, mPaintData);
}

void CVLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    paintImage(painter);
    paintAnnotations(painter);
}

