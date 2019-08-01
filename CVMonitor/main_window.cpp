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

QPixmap pixmapFrom(const QJsonValue &val)
{
  auto const encoded = val.toString().toLatin1();
  QPixmap p;
  p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
  return p;
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

    int max_columns = round( sqrt(app_config::kCVMonitorsCount));
    auto layout = new QGridLayout(ui->mMonitorsWidget);
    for(int i = 0; i < app_config::kCVMonitorsCount; ++i)
    {
        auto caption = "CVMonitor #" + QString::number(i);
        auto monitor = new CVMonitor(caption, i, this);
        auto settings = dynamic_cast<app_config::MonitorSettings*>(CONFIG->mSettings.mCV.mMonitorSettings[i]);

        monitor->bindToSetting(settings);

        mMonitors.push_back(monitor);
        int row = i % max_columns;
        int col = i / max_columns;
        layout->addWidget(monitor, row, col);
    }
}

void MainWindow::handleRequest(QHttpRequest *req, QHttpResponse *resp)
{
   auto responder = new Responder(req, resp, mMonitors);
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
    QJsonParseError error;
    auto obj = QJsonDocument::fromJson(mData, &error).object();

    //qDebug() << obj << mData << (int)error.error;

    auto team = obj["team"].toInt();
    auto task = obj["task"].toString();
    auto evaluated = obj["evaluated"].toObject();
    auto reference = obj["reference"].toObject();
    auto frame = obj["frame"];
    auto result = obj["result"].toString();

    QVariantMap data;
    for(auto& monitor : mMonitors)
    {
        if( (monitor->getSetting()->mTaskType == task) && (monitor->getSetting()->mTeam == team) )
        {
            monitor->setResult(result);
            monitor->setAnnotations(CocoRootItem(reference), CocoRootItem(evaluated));
            auto pixmap = monitor->setFrame(frame);
            data["url"] = (QString)monitor->getSetting()->mSource;
            qDebug() <<"=============================================================";
            qDebug() <<"frame: " << pixmap;
            qDebug() <<"team: " << team;
            qDebug() <<"task: " << task;
            qDebug() <<"evaluated: " << evaluated;
            qDebug() <<"reference: " << reference;
            qDebug() <<"result: " << result;
        }
    }

    m_resp->setHeader("Content-Type", "text/html");
    m_resp->writeHead(200);
    m_resp->end(QJsonDocument::fromVariant(data).toJson());
}
