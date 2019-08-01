#include "cv_monitor.h"
#include "ui_cv_monitor.h"
#include "app_configurator.h"
#include "app_constants.h"
#include <QFileDialog>
#include <chrono>

using namespace app_config;

CVMonitor::CVMonitor(QWidget *parent)
    : CVMonitor("CVMonitor", 0, parent)
{
}

CVMonitor::CVMonitor(const QString &caption, int id, QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::CVMonitor)
    , mCaption(caption)
    , mId(id)
    , mFramesCount(0)
{
    ui->setupUi(this);
    initialize();
}

CVMonitor::~CVMonitor()
{
    delete ui;
}

void CVMonitor::bindToSetting(app_config::MonitorSettings *setting)
{
    mSetting = setting;
    ui->mSourceEdit->bindToSetting(&setting->mSource);
    ui->mTaskTypeComboBox->bindToSetting(&setting->mTaskType);
    ui->mTeamSpinBox->bindToSetting(&setting->mTeam);
}

void CVMonitor::openSource()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Video Source"), "/home/adzhaganian/Videos/test", tr("Video Files (*.avi)"));
    if(!fileName.isEmpty())
    {
        mSetting->mSource =fileName;
    }
}

void CVMonitor::setAnnotations(const CocoRootItem &ref_item, const CocoRootItem &eval_item)
{
    ui->mView->setReferenceAnnotations(ref_item);
    ui->mView->setEvaluatedAnnotations(eval_item);
}

void CVMonitor::setFrame(const QPixmap &pixmap)
{
    mFramesCount++;
    if(!pixmap.isNull())
        ui->mView->setPixmap(pixmap);
}

QPixmap CVMonitor::setFrame(const QJsonValue &value)
{
    auto const encoded = value.toString().toLatin1();
    QPixmap p;
    p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
    setFrame(p);
    return p;
}

void CVMonitor::setResult(const QString &result)
{
    mResult = result;
    refreshHeader();
}

void CVMonitor::initialize()
{
    ui->mCaption->setText(mCaption);
    ui->mSourceEdit->clearFocus();

    connect(ui->mButtonOpen,&QPushButton::clicked, this, &CVMonitor::openSource);

    startTimer(1000);
}

void CVMonitor::refreshHeader()
{
    ui->mCaption->setText(QString("%1 FPS: %2 Res: %3").arg(mCaption).arg(mFps).arg(mResult));
}

MonitorSettings *CVMonitor::getSetting() const
{
    return mSetting;
}


void CVMonitor::timerEvent(QTimerEvent *event)
{
    static auto time      = std::chrono::system_clock::now().time_since_epoch();
    auto new_time      = std::chrono::system_clock::now().time_since_epoch();
    auto delta_time_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count() / 1000000;
    if(delta_time_ms > 0)
    {
        mFps= (mFramesCount * 1000) / delta_time_ms;

        refreshHeader();
        time = new_time;
        mFramesCount = 0;
    }
}
