#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include <QObject>
#include <QtTest>
#include <QDesktopServices>
#include <QString>
#include <QHash>
#include <QUrlInfo>

#include "dropbox/qtdropbox.h"

class DropBoxClient : public QObject
{
    Q_OBJECT
public:
    explicit DropBoxClient(QObject *parent = 0);
    ~DropBoxClient();
    QString getCurrentPath() const { return curPath; }
    QString getUrlStr(const QUrl &url);
    bool isDir(const QString &str) const;
    QString downloadFile(const QString &filename);
    void uploadFile(const QString &filename);
    void setCurrentPath(QString &path);

signals:

    void cmdAddToList(const QDropboxFileInfo &fileInfo);
    void currentPathChanged(const QString &path);

public slots:

    void authorizeApplication();
    bool connectDropbox(QDropbox::OAuthMethod m);
    void changeDir(const QString &dir);
private:
    QDropbox *m_dropbox;
    QStringList files;
    QUrl url;
    bool connected;
    QFile *file;
    QString curPath;
    QHash<QString, bool> isDirHash;
    void getList(const QString path);
    QDropboxFileInfo curPathInfo;


};

#endif // DROPBOXCLIENT_H
