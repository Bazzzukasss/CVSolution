#ifndef CVMONITOR_H
#define CVMONITOR_H

#include <QFrame>
#include "app_settings.h"
#include "cv_logic.h"
#include "coco_item.h"

namespace Ui {
class CVMonitor;
}

class CVMonitor : public QFrame
{
    Q_OBJECT

public:
    explicit CVMonitor(const QString& caption, int id, QWidget *parent = nullptr);
    explicit CVMonitor(QWidget *parent = nullptr);
    ~CVMonitor();

    void bindToSetting(app_config::MonitorSettings* setting);
    void openSource();
    void setSource(const QString& source);
    void setSourceType(const QString &type);
    void setPlaybackAction(PlaybackAction action);
    void setCurrentFrame(const QPixmap& pixmap);
    void propagateFrame(const QPixmap &pixmap);
    void setAnnotations(const CocoRootItem &ref_item, const CocoRootItem &eval_item);
    void flashFrame();
    const QPixmap& getCurrentFrame() const;
    const QJsonValue& getCurrentFrameJson() const;

    void refreshView(const QPixmap &pixmap);
    app_config::MonitorSettings* getSetting() const;

signals:
    void signalFrameChanged(const QPixmap& pixmap);
    void signalFrameReady(const QPixmap& pixmap);

private:
    Ui::CVMonitor *ui;
    CVLogic* mLogic;
    QString mCaption;
    int mId;

    void initialize();
    void refreshState(PlaybackState state);
    void refreshSource(const QString& source, const QString &source_type);
};

#endif // CVMONITOR_H
