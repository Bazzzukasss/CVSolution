#include "main_window.h"
#include <QApplication>
#include "cv_logic.h"

void registerMetaTypes()
{
    qRegisterMetaType<PlaybackState>();
    qRegisterMetaType<PlaybackAction>();
}

int main(int argc, char *argv[])
{
    registerMetaTypes();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
