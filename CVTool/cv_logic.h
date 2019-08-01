#ifndef CVLOGIC_H
#define CVLOGIC_H

#include <QObject>
#include <mutex>
#include <atomic>
#include <QImage>
#include <QPixmap>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QJsonValue>
#include "app_settings.h"

enum class PlaybackAction{kPlay, kStop, kPause, kResume};
enum class PlaybackState{kNone, kPlaying, kStopped, kPaused};

class CVLogic : public QObject
{
    Q_OBJECT
public:
    explicit CVLogic(int id, QObject *parent = nullptr);
    ~CVLogic();

    bool setSource(const QString& source);
    bool setSourceType(const QString& type);
    void setPlaybackAction(PlaybackAction action);
    void setCurrentFrame(const QPixmap& pixmap);
    void setSetting(app_config::MonitorSettings* setting);

    app_config::MonitorSettings* getSetting() const;
    const QString& getSourceType() const;
    const QPixmap& getCurrentFrame() const;
    const QJsonValue& getCurrentFrameJson();
    double getFPS() const;
    int getDeltaTime() const;

    void requestNextFrame();

signals:
    void signalPlaybackStateChanged(PlaybackState state);
    void signalSourceChanged(const QString& source, const QString& source_type);
    void signalFrameChanged(const QPixmap& pixmap);
    void signalFrameReady(const QPixmap& pixmap);

private:
    int mId;
    std::mutex mMutex;
    double mFPS;
    int mDeltaTime;
    int mFramesCount;
    volatile std::atomic_bool mIsNeedToDeactivateThread;
    volatile std::atomic_bool mIsThreadActive;
    volatile std::atomic_bool mIsNeedNextFrame;

    PlaybackState mPlaybackState;
    QString mSource;
    QString mSourceType;
    QPixmap mCurrentFrame;
    QJsonValue mCurrentFrameJson;

    int mFrameRate;
    cv::VideoCapture mCapture;
    app_config::MonitorSettings* mSetting;

    void initialize();
    void activate();
    void deactivate();
    void dataProcedure();
    bool applySource();
    bool openSource();
    QJsonValue convertFrameToJson(const QPixmap &p) const;
};

Q_DECLARE_METATYPE(PlaybackAction);
Q_DECLARE_METATYPE(PlaybackState);

#endif // CVLOGIC_H
