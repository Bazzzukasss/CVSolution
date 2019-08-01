#ifndef CVMONITOR_H
#define CVMONITOR_H

#include <QFrame>
#include "app_settings.h"
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
    void setAnnotations(const CocoRootItem &ref_item, const CocoRootItem &eval_item);
    void setFrame(const QPixmap &pixmap);
    QPixmap setFrame(const QJsonValue& value);
    void setResult(const QString& result);

    app_config::MonitorSettings* getSetting() const;
private:
    Ui::CVMonitor *ui;
    QString mCaption;
    double mFps;
    QString mResult;
    app_config::MonitorSettings* mSetting;
    int mId;
    int mFramesCount;

    void initialize();
    void refreshHeader();

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event) override;
};

#endif // CVMONITOR_H
