#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
    setupUi(this);
    m_logger = new Logger(this);

    /* center the window */
    QRect rect = QApplication::desktop()->availableGeometry(this);
    move(rect.center() - this->rect().center());

    m_hasImageLoaded = false;
    tabWidget->clear();
    tabWidget->addTab(new ImageWidget(tabWidget), QString("No image selected"));

    connectSignals();
}
MainWindow::~MainWindow()
{
    delete(contextMenu);
    delete(m_logger);
}

void MainWindow::addImageTab(QPointer<Image> img)
{
    closeTabByName(img->shortName());

    ImageWidget* w;
    if(!m_hasImageLoaded)
    {
        w = (ImageWidget*)tabWidget->currentWidget();
        w->setPixmap(*img);
        tabWidget->setTabText(tabWidget->currentIndex(), img->shortName());
    }
    else
    {
        w = new ImageWidget(tabWidget);
        w->setPixmap(*img);
        tabWidget->addTab(w, img->shortName());
    }
    w->setContextMenu(contextMenu);
    closeTabButton->setVisible(true);
    m_imageMap[img->shortName()]=img;
    setEnabledImageActions(true);
    m_hasImageLoaded = true;
    tabWidget->setCurrentIndex(tabWidget->count()-1);
}

void MainWindow::closeCurrentTab()
{

    if(tabWidget->count()==1)
    {
        m_hasImageLoaded = false;
        setEnabledImageActions(false);
        closeTabButton->setVisible(false);
    }
    tabWidget->removeTab(tabWidget->currentIndex());

    if(!m_hasImageLoaded)
        tabWidget->addTab(new ImageWidget(tabWidget), "No image selected");
}

void MainWindow::closeTabByName(QString name)
{
    if( !m_imageMap.contains(name) )
        return;

    int i =0;
    while(i<tabWidget->count())
    {
        if(tabWidget->tabText(i)==name)
        {
            tabWidget->removeTab(i);
            delete(m_imageMap[name]);
            m_imageMap.remove(name);
            return;
        }
        i++;
    }
}

void MainWindow::setEnabledImageActions(const bool value)
{
    decodeButton->setEnabled(value);
    actionDecode->setEnabled(value);
    encodeButton->setEnabled(value);
    actionEncode->setEnabled(value);
    takePhotoButton->setEnabled(value);
    actionTakePhoto->setEnabled(value);
    actionTakePhoto->setEnabled(value);
    actionCloseCurrent->setEnabled(value);
    actionCopy->setEnabled(value);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        for(int i=0; i<urls.size(); i++)
        {
            if(urls.at(i).toLocalFile().length()>0)
                newImage(urls.at(i).toLocalFile());
        }
    }
    else
    {
        QMessageBox::warning(this, tr("HiddenDragon Warning"),
                             "The dropped object doesn't have any url.");
    }

}

QPointer<Image> MainWindow::currentImage()
{
    QString key = tabWidget->tabText(tabWidget->currentIndex());
    if(m_imageMap.contains(key))
        return m_imageMap[key];
    else
        return NULL;
}

void MainWindow::newImage(QString url)
{
    if(QFile::exists(url))
    {
        QPointer<Image> img = new Image(url);
        if(img->isNull())
        {
            QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                 "Cannot load '" + url + "'\n file is not a valid image.");
            return;
        }
        addImageTab(img);
    }
    else
        m_logger->warning("Cannot load '" + url + "'\n file doesn't exist !");
}

void MainWindow::openFile()
{
    setCursor(Qt::WaitCursor);
    QFileDialog dialog(this, tr("Open Images"));
    dialog.setFilter(tr("Image Files (*.png *.jpg .jpeg *.bmp *.tiff)"));
    dialog.setViewMode(QFileDialog::List);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setDirectory(Controller::config.get("output"));
    if(dialog.exec())
    {
        QStringList fileNames = dialog.selectedFiles();
        for(int i=0; i<fileNames.size(); i++)
        {
            newImage(fileNames.at(i));
        }
    }
    setCursor(Qt::ArrowCursor);
}

void MainWindow::paste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        QPointer<Image> img = new Image(qvariant_cast<QPixmap>(mimeData->imageData()));
        img->setShortName("pasted_image.img");
        addImageTab(img);
    } else {
        QMessageBox::warning(this, tr("HiddenDragon Warning"),
                             "Cannot paste clipboard's data: not an image!");
    }
}

void MainWindow::copy()
{
    if (m_hasImageLoaded)
    {

        //QMimeData *data = new QMimeData;
        //data->setImageData(currentImage()->toImage());
        //clipboard->setMimeData(data, QClipboard::Clipboard);
    }
}


void MainWindow::execEncodeDialog()
{
    encodeDialog.setImage(currentImage());
    encodeDialog.exec();
}

void MainWindow::execDecodeDialog()
{
    decodeDialog.setImage(currentImage());
    decodeDialog.exec();
}

void MainWindow::execTakePhotoDialog()
    {
        QProcess::startDetached("D:\\WorkSpace\\Projects\\camera\\release\\camera.exe");
    }

void MainWindow::connectSignals()
{


    // Button actions
    connect(encodeButton, SIGNAL(pressed()), this, SLOT(execEncodeDialog()));
    connect(actionEncode, SIGNAL(triggered()), this, SLOT(execEncodeDialog()));
    connect(takePhotoButton, SIGNAL(pressed()), this, SLOT(execTakePhotoDialog()));
    connect(actionTakePhoto, SIGNAL(triggered()), this, SLOT(execTakePhotoDialog()));
    connect(decodeButton, SIGNAL(pressed()), this, SLOT(execDecodeDialog()));
    connect(actionDecode, SIGNAL(triggered()), this, SLOT(execDecodeDialog()));

    // Menu actions
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(actionCloseCurrent, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
  //  connect(actionPreferences, SIGNAL(triggered()), &preferenceDialog, SLOT(exec()));
    connect(actionAbout, SIGNAL(triggered()), &aboutDialog, SLOT(exec()));

    // Others
    connect(&encodeDialog, SIGNAL(encodedImage(QString)), this, SLOT(newImage(QString)));

    contextMenu = new QMenu(this);
    contextMenu->addAction(actionCloseCurrent);
    contextMenu->addSeparator();
    contextMenu->addAction(actionEncode);
    contextMenu->addAction(actionDecode);
    contextMenu->addAction(actionTakePhoto);
    contextMenu->addSeparator();
    contextMenu->addAction(actionCopy);

    // close tab button
    closeTabButton = new QPushButton(this);
    closeTabButton->setIcon(QIcon(":/menu/img/close.png"));
    closeTabButton->setFlat(true);
    tabWidget->setCornerWidget(closeTabButton);
    closeTabButton->setVisible(false);
    connect(closeTabButton, SIGNAL(pressed()), this, SLOT(closeCurrentTab()));
}


