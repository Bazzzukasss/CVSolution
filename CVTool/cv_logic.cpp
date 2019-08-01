#include "cv_logic.h"
#include <thread>
#include <iostream>
#include <QDebug>
#include <chrono>
#include <QBuffer>
#include "app_constants.h"
using namespace std;
using namespace app_config;

CVLogic::CVLogic(int id, QObject *parent)
    : QObject(parent)
    , mId(id)
    , mFPS(0)
    , mDeltaTime(0)
    , mFramesCount(0)
    , mIsNeedToDeactivateThread(false)
    , mIsThreadActive(false)
    , mIsNeedNextFrame(true)
    , mPlaybackState(PlaybackState::kNone)
    , mSource("")
{
    initialize();
    activate();
}

CVLogic::~CVLogic()
{
    deactivate();
}

bool CVLogic::setSource(const QString &source)
{
    qDebug()<<"setSource:" << source;
    mSource = source;
    return applySource();
}

bool CVLogic::setSourceType(const QString &type)
{
    qDebug()<<"setSourceType:" << type;
    mSourceType = type;
    return applySource();
}

bool CVLogic::applySource()
{
    emit signalSourceChanged(mSource, mSourceType);

    if(mSourceType == kSourceFile)
    {
        mCapture.open(mSource.toStdString());
        return openSource();
    }

    if(mSourceType == kSourceIp)
    {
        mCapture.open(mSource.toStdString());
        return openSource();
    }

    if(mSourceType == kSourceWebCam)
    {
        mCapture.open(0);
        return openSource();
    }

    if(mSourceType == kSourcePrimary)
    {
        if(mId != 0)
        {
            mCapture.release();
        }
        return true;
    }

    return false;
}

bool CVLogic::openSource()
{
    if (mCapture.isOpened())
    {
        if(mPlaybackState !=  PlaybackState::kStopped)
            setPlaybackAction(PlaybackAction::kStop);

        mFrameRate = (int) mCapture.get(CV_CAP_PROP_FPS);
        qDebug()<<"Framerate:" << mFrameRate;
        return true;
    }
    else
    {
        qDebug()<<"Failed.";
        mPlaybackState = PlaybackState::kNone;
        emit signalPlaybackStateChanged(mPlaybackState);
        return false;
    }
}

QJsonValue CVLogic::convertFrameToJson(const QPixmap &p) const
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    //buffer.open(QIODevice::WriteOnly);

    //auto time      = std::chrono::system_clock::now().time_since_epoch();
    p.save(&buffer, "PNG", mSetting->mCompression);
    //auto new_time      = std::chrono::system_clock::now().time_since_epoch();
    //auto delta_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count();
    //qDebug()<<"To Buff:"<<delta_time_ns/1000000;

    return {QLatin1String(byteArray.toBase64())};
}

void CVLogic::setPlaybackAction(PlaybackAction action)
{
    std::unique_lock<std::mutex> locker(mMutex);

    switch(action)
    {
        case PlaybackAction::kPlay:
            setSource(mSource);
            mPlaybackState =  PlaybackState::kPlaying;
        break;
        case PlaybackAction::kStop:
            mPlaybackState =  PlaybackState::kStopped;
            mCapture.release();
        break;
        case PlaybackAction::kPause:
            mPlaybackState =  PlaybackState::kPaused;
        break;
        case PlaybackAction::kResume:
            mPlaybackState =  PlaybackState::kPlaying;
        break;
    }

    qDebug()<<"setPlaybackAction:" << (int)action;
    emit signalPlaybackStateChanged(mPlaybackState);
}

void CVLogic::setCurrentFrame(const QPixmap &pixmap)
{
    mCurrentFrame = pixmap;

    if(mSetting->mAutoPlay)
        emit signalFrameChanged(pixmap);
    else
        emit signalFrameReady(pixmap);
 }

void CVLogic::setSetting(MonitorSettings *setting)
{
    mSetting = setting;
}

MonitorSettings *CVLogic::getSetting() const
{
    return mSetting;
}

const QString &CVLogic::getSourceType() const
{
    return mSourceType;
}

const QPixmap &CVLogic::getCurrentFrame() const
{
    return mCurrentFrame;
}

const QJsonValue &CVLogic::getCurrentFrameJson()
{
    mCurrentFrameJson = convertFrameToJson(mCurrentFrame);
    return mCurrentFrameJson;
}

double CVLogic::getFPS() const
{
    return mFPS;
}

int CVLogic::getDeltaTime() const
{
    return mDeltaTime / 1000000;
}

void CVLogic::requestNextFrame()
{
    mIsNeedNextFrame = true;
    while(mIsNeedNextFrame && mPlaybackState == PlaybackState::kPlaying)
    {
        this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}

void CVLogic::initialize()
{
}

void CVLogic::activate()
{
    std::thread thread(&CVLogic::dataProcedure, this);
    thread.detach();
    mIsThreadActive = true;
    cout << "Thread has been activated." << endl;
}

void CVLogic::deactivate()
{
    if (mIsThreadActive)
    {
        // request disactivation and notify thread
        mIsNeedToDeactivateThread = true;

        // waiting while thread is not stoped
        while (mIsThreadActive)
        {
            this_thread::yield();
        }

        // thread is disactivated
        mIsNeedToDeactivateThread = false;
    }
    mCapture.release();
}

void CVLogic::dataProcedure()
{
    cv::Mat RGBframe;
    cv::Mat frame;
    QImage image;
    auto time = std::chrono::system_clock::now().time_since_epoch();

    while(!mIsNeedToDeactivateThread)
    {
        {
            unique_lock<mutex> locker(mMutex);

            if(mPlaybackState == PlaybackState::kPlaying)
            {
                if((mSetting->mAutoPlay) || ((!mSetting->mAutoPlay) && mIsNeedNextFrame))
                {
                    auto new_time      = std::chrono::system_clock::now().time_since_epoch();
                    mDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count();
                    mFPS = 1000000000 / mDeltaTime;

                    //qDebug()<<"Video:" << mDeltaTime/1000000 <<mFramesCount;

                    if((mDeltaTime < chrono::nanoseconds(1000000000 / mFrameRate).count()) && mSetting->mAutoPlay)
                        continue;
                    else
                        time = new_time;

                    if (!mCapture.read(frame))
                    {
                        locker.unlock();
                        setPlaybackAction(PlaybackAction::kStop);
                        continue;
                    }

                    if (frame.channels()== 3){
                        cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
                        image = QImage((const unsigned char*)(RGBframe.data), RGBframe.cols, RGBframe.rows, QImage::Format_RGB888);
                    }
                    else
                    {
                        image = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_Indexed8);
                    }

                    setCurrentFrame(QPixmap::fromImage(image));
                    mIsNeedNextFrame = false;

                    locker.unlock();
                }
            }
        }
        //this_thread::yield();
        this_thread::sleep_for(std::chrono::nanoseconds(10000));
    }

    cout << "Thread has been stopped." << endl;
    mIsThreadActive = false;
}
