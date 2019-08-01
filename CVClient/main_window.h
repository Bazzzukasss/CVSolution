#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "requester.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Requester mRequester;
    bool mIsNeedNextFrame;
    int mDeltaTime;
    int mFPS;

    void sendAnnotations();

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event) override;
};

#endif // MAIN_WINDOW_H
