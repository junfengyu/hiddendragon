#ifndef DROPBOXDIALOG_H
#define DROPBOXDIALOG_H

#include <QDialog>
#include <QUrlInfo>
#include <QString>
#include "ui_dropboxdialog.h"
#include "dropboxclient.h"

class DropboxDialog : public QDialog, public Ui::DropboxDialog
{
    Q_OBJECT

public:
    explicit DropboxDialog(QWidget *parent = 0);
    ~DropboxDialog();
    void connectSignals();
    QString getCurrentPath() const;
    bool isDir(const QString &str) const;
private:
    void clearMemory(QList<QListWidgetItem*> &listItemCtn);
signals:
    void encodedFileDownloaded(QString encodedFileName);
private slots:
    void dropboxDialogQuit();
    void callDownload();
    void callUpload();
    void DoubleClicked(QListWidgetItem *item);
    void addToList(const QDropboxFileInfo &fileInfo);
    QString downloadFile(const QString &filename);
    void changeToParrent();
    void switchDownloadBtn(QListWidgetItem *item);
    void dropboxConnect();
    void uploadFile(const QString &filename);
private:
    DropBoxClient dropboxClient;
    QList<QListWidgetItem*> listItemCtn;
};

#endif // DROPBOXDIALOG_H
