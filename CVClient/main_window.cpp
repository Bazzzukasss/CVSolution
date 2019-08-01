#include "main_window.h"
#include "ui_main_window.h"
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QCheckBox>
#include <chrono>

QPixmap pixmapFrom(const QJsonValue &val)
{
  auto const encoded = val.toString().toLatin1();
  QPixmap p;
  p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
  return p;
}

QJsonValue jsonValFromPixmap(const QPixmap &p)
{
  QBuffer buffer;
  buffer.open(QIODevice::WriteOnly);
  p.save(&buffer, "PNG");
  return {QLatin1String(buffer.data().toBase64())};
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mIsNeedNextFrame(true)
{
    ui->setupUi(this);

    //mRequester.initRequester("127.0.0.1", 8080, nullptr);
    mRequester.initRequester("172.22.41.37", 8080, nullptr);

    startTimer(5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendAnnotations()
{
    static double alfa{0};
    auto s = ui->mView->size();
    int x = (double)s.width() * (1.5 + sin(alfa));
    int y = (double)s.height() * (1.5 + cos(alfa));
    alfa+=0.1;
    if(alfa > 3.14 * 2.0)
        alfa = 0.0;

    mRequester.sendRequest(
    {},
    [&](const QJsonObject& obj){
        //qDebug()<<"POST Success";
        qDebug() << obj;
        mIsNeedNextFrame = true;
        if(ui->mShowCheckBox->isChecked())
        {
            auto pixmap = pixmapFrom(obj["frame"]);
            if(!pixmap.isNull())
                ui->mView->setPixmap(pixmap);
        }
    },

    [&](const QJsonObject &){
        qDebug()<<"POST Error";
        mIsNeedNextFrame = true;
    },

    Requester::Type::POST,
    {
        {"frame", jsonValFromPixmap(QPixmap("://res/frame.png"))},
        {"team", ui->mTeamSpinBox->value()},
        {"task", ui->mTaskComboBox->currentText()},
        {"reference", QVariantMap{{"annotations", QVariantList{ QVariantMap{{"id", 1},{"x",x},{"y",y}}} }}},
        {"evaluated", QVariantMap{{"annotations", QVariantList{ QVariantMap{{"id", 1},{"x",x+5},{"y",y+5}}} }}},
        {"result", "Good / Bad"},
    }
    );
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    static auto time      = std::chrono::system_clock::now().time_since_epoch();

    if(ui->mRunCheckBox->isChecked() && mIsNeedNextFrame)
    {
        auto new_time      = std::chrono::system_clock::now().time_since_epoch();
        auto delta_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count();

        ui->mDeltaTimeLabel->setText(QString("Time: %1 ms").arg(delta_time_ns/1000000));
        ui->mFPSLabel->setText(QString("FPS: %1").arg(1000000000 / delta_time_ns));

        time = new_time;

        mIsNeedNextFrame = false;
        sendAnnotations();
    }
}
