#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qhttpserver.h"

class CVMonitor;

namespace Ui {
class MainWindow;
}

/// Responder

class Responder : public QObject
{
    Q_OBJECT

public:
    Responder(QHttpRequest *req, QHttpResponse *resp, const QVector<CVMonitor*> monitors);
    ~Responder();

signals:
    void done();
    void finished();

private slots:
    void accumulate(const QByteArray &data);
    void reply();

private:
    QScopedPointer<QHttpRequest> m_req;
    QHttpResponse *m_resp;
    QByteArray mData;
    QVector<CVMonitor*> mMonitors;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<CVMonitor*> mMonitors;
    QHttpServer mServer;
    QMap<QHttpRequest*,QHttpResponse*> req;


    void handleRequest(QHttpRequest *req, QHttpResponse *resp);
    void initialize();
    void storeConfig();
    void restoreConfig();
};

#endif // MAINWINDOW_H
