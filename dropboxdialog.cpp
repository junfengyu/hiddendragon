
#include <QIcon>
#include <QPixmap>
#include <QListWidgetItem>
#include <image.h>
#include "dropboxdialog.h"
#include "dropbox/qdropboxfileinfo.h"
#include "ui_dropboxdialog.h"
#include "mainwindow.h"

extern QMap<QString, QPointer<Image> > m_outputImageFullNameMap;

DropboxDialog::DropboxDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    listWidget->clear();
    listWidget->setEnabled(true);
    connectSignals();


}

DropboxDialog::~DropboxDialog()
{

}

void DropboxDialog::connectSignals()
{
    connect(quitButton,SIGNAL(clicked()),this,SLOT(dropboxDialogQuit()));
    connect(downloadButton,SIGNAL(clicked()),this,SLOT(callDownload()));
    //connect(upButton, SIGNAL(clicked()), this, SLOT(changeToParrent()));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(DoubleClicked(QListWidgetItem*)));
    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(switchDownloadBtn(QListWidgetItem*)));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(dropboxConnect()));
    connect(uploadButton,SIGNAL(clicked()),this,SLOT(callUpload()));
    connect(&dropboxClient, SIGNAL(cmdAddToList(QDropboxFileInfo)), this, SLOT(addToList(QDropboxFileInfo)));

}

void DropboxDialog::changeToParrent()
{
    clearMemory(listItemCtn);
    listWidget->clear();
    listItemCtn.clear();

}

void DropboxDialog::clearMemory(QList<QListWidgetItem*> &listItemCtn)
{
    QListWidgetItem *item;
    QList<QListWidgetItem*>::iterator it;

    it = listItemCtn.begin();
    while (it != listItemCtn.end())
    {
        item = *it;
        if (item != NULL)
        {
            delete item;
            item = NULL;
        }
        ++it;
    }

}

void DropboxDialog::switchDownloadBtn(QListWidgetItem *item)
{
    QString filename;

    filename = item->text();
    if (this->isDir(filename))
    {
        downloadButton->setEnabled(false);
    }
    else
    {
        downloadButton->setEnabled(true);
    }
}

void DropboxDialog::callDownload()
{
    QString str;

    str = listWidget->currentItem()->text();	/* filename */
    if (isDir(str) == false)
    {
        QString encodedFileName=downloadFile(str);
        emit encodedFileDownloaded(encodedFileName);
    }
}



QString DropboxDialog::downloadFile(const QString &filename)
{
    QString fileName=dropboxClient.downloadFile(filename);
    return fileName;
}

void DropboxDialog::callUpload()
{
     foreach(QString eachFilePath, m_outputImageFullNameMap.keys())
        uploadFile(eachFilePath);
}

void DropboxDialog::uploadFile(const QString &filename)
{
     dropboxClient.uploadFile(filename);
}

void DropboxDialog::dropboxDialogQuit()
{
     close();
}

void DropboxDialog::dropboxConnect()
{
    dropboxClient.connectDropbox(QDropbox::Plaintext);
}

bool DropboxDialog::isDir(const QString &str) const
{
    return (dropboxClient.isDir(str));
}



void DropboxDialog::addToList(const QDropboxFileInfo &fileInfo)
{
   // QListWidgetItem *item = new QListWidgetItem(listWidget);
    int i=0;
    QString longPath;
    QList<QDropboxFileInfo> filesInfo;
    if(fileInfo.isDir())
    {
        filesInfo = fileInfo.contents();
        for (i = 0; i < filesInfo.size(); i++) {
            QListWidgetItem *item = new QListWidgetItem(listWidget);
            longPath=filesInfo.at(i).path();
            item->setText(longPath);
            const QIcon icon = QIcon(":/icons/Picture.png");
            item->setIcon(icon);
            listItemCtn.append(item);
            listWidget->addItem(item);

        }

    }

}

void DropboxDialog::DoubleClicked(QListWidgetItem *item)
{
    QString dir, filename, str;

    str = item->text();
    if (isDir(str))
    {
        dir = str;

        clearMemory(listItemCtn);
        listWidget->clear();
        listItemCtn.clear();
        //ftpclient.changeDir(dir); // unimplemented!!!
    }
    else
    {
        filename = str;
        downloadFile(filename);
    }
}
