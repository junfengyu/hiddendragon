#include "mainwindow.h"
#include <QApplication>

#include "logger.h"
#include "mainwindow.h"

#include "controller.h"


void showMessage(QString msg, Logger& logger)
{
    logger.warning(msg);
    QMessageBox mb("HiddenDragon Warning",
                  msg, QMessageBox::Warning, 0, 0, 0);
    mb.exec();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Controller::appPath = qApp->applicationDirPath()+"/";
    Controller::config = Config(Controller::appPath, "hiddendragon");

    Logger::setFileName(Controller::appPath + "hiddendragon.log");
    Logger::setLevel(Controller::config.get("loglevel"));



    if (app.argc() <= 1)
    {
        /* Normal mode */
        if( !Controller::config.contains("output") || Controller::config.get("output").trimmed() == "")
            // default output is the user home
            Controller::config.set("output", QDir::homePath()+"/");

        MainWindow mainWin;
        mainWin.show();
        return app.exec();
    }


}
