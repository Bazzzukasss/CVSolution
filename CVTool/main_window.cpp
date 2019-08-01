#include "main_window.h"
#include "ui_main_window.h"
#include <QGridLayout>
#include "cv_monitor.h"
#include "app_configurator.h"
#include "app_constants.h"
#include <QPixmap>
#include <QBuffer>
#include <QJsonDocument>
#include <QThread>
#include "qhttprequest.h"
#include "qhttpresponse.h"

QJsonValue jsonValFromPixmap(const QPixmap &p)
{
  QBuffer buffer;
  buffer.open(QIODevice::WriteOnly);
  p.save(&buffer, "PNG");
  return {QLatin1String(buffer.data().toBase64())};
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    restoreConfig();
    initialize();
}

MainWindow::~MainWindow()
{
    storeConfig();
    delete ui;
}

void MainWindow::initialize()
{
    connect(&mServer, &QHttpServer::newRequest, this, &MainWindow::handleRequest);
    mServer.listen(QHostAddress::Any, 8080);

    int max_columns = round(sqrt(app_config::kCVMonitorsCount) + 0.5);
    auto layout = new QGridLayout(ui->mMonitorsWidget);
    for(int i = 0; i < app_config::kCVMonitorsCount; ++i)
    {
        auto caption = i == 0 ? "Primary Monitor" : "CVMonitor #" + QString::number(i);
        auto monitor = new CVMonitor(caption, i, this);
        auto settings = dynamic_cast<app_config::MonitorSettings*>(CONFIG->mSettings.mCV.mMonitorSettings[i]);

        monitor->bindToSetting(settings);
        monitor->setSource(settings->mSource);
        monitor->setSourceType(settings->mSourceType.getDataValue());
        mMonitors.push_back(monitor);
        int row = i / max_columns;
        int col = i % max_columns;
        layout->addWidget(monitor, row, col);

        if(i > 0)
        {
            connect(mMonitors[0], &CVMonitor::signalFrameChanged, monitor, &CVMonitor::propagateFrame);
            connect(mMonitors[0], &CVMonitor::signalFrameReady, monitor, &CVMonitor::propagateFrame);
        }
    }
}

void MainWindow::handleRequest(QHttpRequest *req, QHttpResponse *resp)
{
   auto responder = new Responder(req, resp, mMonitors);
   //auto thread = new QThread();
   //responder->moveToThread(thread);
   //connect(responder, SIGNAL(finished()), thread, SLOT(quit()));
   //connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
   //thread->start();
}

void MainWindow::storeConfig()
{
    CONFIG->mSettings.mGui.mWindowGeometry = saveGeometry();
    CONFIG->mSettings.mGui.mWindowState    = saveState();
    CONFIG->saveConfigurations();
}

void MainWindow::restoreConfig()
{
    CONFIG->loadConfigurations();
    restoreGeometry(CONFIG->mSettings.mGui.mWindowGeometry);
    restoreState(CONFIG->mSettings.mGui.mWindowState);
}


/// Responder
Responder::Responder(QHttpRequest *req, QHttpResponse *resp, const QVector<CVMonitor*> monitors)
    : m_req(req)
    , m_resp(resp)
    , mMonitors(monitors)
{
    //qDebug() << "handleRequest : " << m_req->body() << m_req->path();


    connect(req, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
    connect(req, SIGNAL(end()), this, SLOT(reply()));
    connect(m_resp, SIGNAL(done()), this, SIGNAL(finished()));
    connect(m_resp, SIGNAL(done()), this, SLOT(deleteLater()));
}

Responder::~Responder()
{
}

void Responder::accumulate(const QByteArray &data)
{
    mData += data;
}

void Responder::reply()
{
    auto obj = QJsonDocument::fromJson(mData).object();

    auto team = obj["team"].toInt();
    auto task = obj["task"].toString();
    auto evaluated = obj["evaluated"].toObject();
    auto reference = obj["reference"].toObject();

    //qDebug() << obj << task << evaluated << reference;
    QJsonValue newFrame;

    for(auto& monitor : mMonitors)
    {
        if( (monitor->getSetting()->mTaskType == task) && (monitor->getSetting()->mTeam == team) )
        {
            monitor->setAnnotations(CocoRootItem(reference), CocoRootItem(evaluated));
            //auto time      = std::chrono::system_clock::now().time_since_epoch();
            //if(newFrame.isNull())
                newFrame = monitor->getCurrentFrameJson();
            //auto new_time      = std::chrono::system_clock::now().time_since_epoch();
            //auto delta_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count();
            //qDebug()<<"Convert:"<<delta_time_ns/1000000;

            monitor->flashFrame();
        }
    }

    QVariantMap data;
    if(!newFrame.isNull())
    {
        data["frame"] = newFrame;
        data["team"] = team;
        data["task"] = task;
    }

    m_resp->setHeader("Content-Type", "text/html");
    m_resp->writeHead(200);
    m_resp->end(QJsonDocument::fromVariant(data).toJson());
}