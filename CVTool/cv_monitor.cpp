#include "cv_monitor.h"
#include "ui_cv_monitor.h"
#include "app_configurator.h"
#include "app_constants.h"
#include <QFileDialog>

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
    mLogic->setSetting(setting);
    ui->mSourceEdit->bindToSetting(&setting->mSource);
    ui->mSourceTypeComboBox->bindToSetting(&setting->mSourceType);
    ui->mTaskTypeComboBox->bindToSetting(&setting->mTaskType);
    ui->mTeamSpinBox->bindToSetting(&setting->mTeam);
    ui->mAutoplayCheckBox->bindToSetting(&setting->mAutoPlay);
    ui->mQualitySlider->bindToSetting(&setting->mCompression);
}

void CVMonitor::openSource()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Video Source"), "/home/adzhaganian/Videos/test", tr("Video Files (*.avi)"));
    if(!fileName.isEmpty())
    {
        setSource(fileName);
    }
}

void CVMonitor::setSource(const QString &source)
{
    ui->mSourceEdit->setText(source);
    mLogic->setSource(source);
}

void CVMonitor::setSourceType(const QString& type)
{
    mLogic->setSourceType(type);
}

void CVMonitor::setPlaybackAction(PlaybackAction action)
{
    mLogic->setPlaybackAction(action);
}

void CVMonitor::setCurrentFrame(const QPixmap &pixmap)
{
    mLogic->setCurrentFrame(pixmap);
}

void CVMonitor::propagateFrame(const QPixmap &pixmap)
{
    if(mLogic->getSourceType() != kSourcePrimary)
        return ;

    setCurrentFrame(pixmap);
}

void CVMonitor::setAnnotations(const CocoRootItem &ref_item, const CocoRootItem &eval_item)
{
    ui->mView->setReferenceAnnotations(ref_item);
    ui->mView->setEvaluatedAnnotations(eval_item);
}

void CVMonitor::flashFrame()
{
    refreshView(mLogic->getCurrentFrame());
    mLogic->requestNextFrame();
}

const QPixmap &CVMonitor::getCurrentFrame() const
{
    return mLogic->getCurrentFrame();
}

const QJsonValue &CVMonitor::getCurrentFrameJson() const
{
    return mLogic->getCurrentFrameJson();
}

void CVMonitor::initialize()
{
    mLogic = new CVLogic(mId, this);
    ui->mCaption->setText(mCaption);
    ui->mSourceEdit->clearFocus();

    ui->mButtonPlay->setFocusPolicy(Qt::NoFocus);
    ui->mButtonStop->setFocusPolicy(Qt::NoFocus);
    ui->mButtonResume->setFocusPolicy(Qt::NoFocus);
    ui->mButtonPause->setFocusPolicy(Qt::NoFocus);

    connect(ui->mButtonOpen,&QPushButton::clicked, this, &CVMonitor::openSource);
    connect(ui->mButtonPlay,&QPushButton::clicked, [&](){ mLogic->setPlaybackAction(PlaybackAction::kPlay); });
    connect(ui->mButtonStop,&QPushButton::clicked, [&](){ mLogic->setPlaybackAction(PlaybackAction::kStop); });
    connect(ui->mButtonPause,&QPushButton::clicked, [&](){ mLogic->setPlaybackAction(PlaybackAction::kPause); });
    connect(ui->mButtonResume,&QPushButton::clicked, [&](){ mLogic->setPlaybackAction(PlaybackAction::kResume); });
    connect(ui->mSourceEdit, &QLineEdit::editingFinished, [&](){ mLogic->setSource(ui->mSourceEdit->text()); });
    connect(ui->mSourceTypeComboBox, &QComboBox::currentTextChanged, [&](const QString& text){ mLogic->setSourceType(text); });
    //connect(ui->mAutoplayCheckBox, &QCheckBox::stateChanged, [&](int state){ if(state) mLogic->setPlaybackMode(PlaybackMode::kContinuous); else mLogic->setPlaybackMode(PlaybackMode::kByRequest); });

    connect(mLogic, &CVLogic::signalPlaybackStateChanged, this, &CVMonitor::refreshState);
    connect(mLogic, &CVLogic::signalSourceChanged, this, &CVMonitor::refreshSource);
    connect(mLogic, &CVLogic::signalFrameChanged, this, &CVMonitor::refreshView);
    connect(mLogic, &CVLogic::signalFrameChanged, this, &CVMonitor::signalFrameChanged);
    connect(mLogic, &CVLogic::signalFrameReady, this, &CVMonitor::signalFrameReady);
}

void CVMonitor::refreshState(PlaybackState state)
{
    ui->mButtonPlay->setVisible(false);
    ui->mButtonStop->setVisible(false);
    ui->mButtonResume->setVisible(false);
    ui->mButtonPause->setVisible(false);

    switch(state)
    {
        case PlaybackState::kNone:
        break;
        case PlaybackState::kPlaying:
            ui->mButtonStop->setVisible(true);
            ui->mButtonPause->setVisible(true);
        break;
        case PlaybackState::kPaused:
            ui->mButtonResume->setVisible(true);
            ui->mButtonStop->setVisible(true);
        break;
        case PlaybackState::kStopped:
            ui->mButtonPlay->setVisible(true);
        break;
    }
}

void CVMonitor::refreshSource(const QString &source, const QString & source_type)
{
    QString header;
    bool isOpenButtonVisible{false};
    bool isSourceEditVisible{false};
    bool isButtonsVisible{false};

    if(source_type == kSourceFile)  { header = source_type + ": " + source; isOpenButtonVisible = true;  isSourceEditVisible = true;  isButtonsVisible = true; };
    if(source_type == kSourceIp)    { header = source_type + ": " + source; isOpenButtonVisible = false; isSourceEditVisible = true;  isButtonsVisible = true; };
    if(source_type == kSourceWebCam){ header = source_type;                 isOpenButtonVisible = false; isSourceEditVisible = false; isButtonsVisible = true; };
    if(source_type == kSourcePrimary)  { header = source_type;                 isOpenButtonVisible = false; isSourceEditVisible = false; isButtonsVisible = false; };

    ui->mSource->setText(header);
    ui->mButtonOpen->setVisible(isOpenButtonVisible);
    ui->mSourceEdit->setVisible(isSourceEditVisible);
    ui->mButtonsFrame->setVisible(isButtonsVisible);
}

void CVMonitor::refreshView(const QPixmap &pixmap)
{
    ui->mView->setPixmap(pixmap);
    ui->mCaption->setText(QString("%1, FPS: %2(%3ms)").arg(mCaption).arg(mLogic->getFPS()).arg(mLogic->getDeltaTime()));
}

MonitorSettings *CVMonitor::getSetting() const
{
    return mLogic->getSetting();
}
