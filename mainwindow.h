#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "encodedialog.h"
#include "decodedialog.h"
#include "imagewidget.h"
#include "logger.h"
#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
    private:
        EncodeDialog encodeDialog;

        DecodeDialog decodeDialog;

        AboutDialog aboutDialog;

        QMenu* contextMenu;
        QPushButton* closeTabButton;

        QMap<QString, QPointer<Image> > m_imageMap;
        bool m_hasImageLoaded;

        QPointer<Logger> m_logger;
    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        void connectSignals();
        void setEnabledImageActions(const bool value);
        QPointer<Image> currentImage();
        void closeTabByName(QString name);

    private slots:
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
        void closeCurrentTab();
        void addImageTab(QPointer<Image> img);

        void paste();
        void copy();
        void execDecodeDialog();
        void execEncodeDialog();

        void execTakePhotoDialog();

        void newImage(QString);
        void openFile();
};


#endif // MAINWINDOW_H
