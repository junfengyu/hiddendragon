
#include <QWidget>
#include <QMessageBox>
#include <QDir>
#include <QProgressDialog>
#include <QProcess>
#include "dropboxclient.h"
#include "dropbox/qdropboxfile.h"
//#include "dialog.h"


const QString APP_KEY="hca6eo97efahfi5";
const QString APP_SECRET="t9ue5abgn4gprb0";

DropBoxClient::DropBoxClient(QObject *parent) :
    QObject(parent)
{
     m_dropbox=new QDropbox(APP_KEY, APP_SECRET);

}
    /**
     * @brief Prompt the user for authorization.
     */
void DropBoxClient::authorizeApplication()
{
        QTextStream strout(stdout);
        QTextStream strin(stdin);

        strout << "URL: " << m_dropbox->authorizeLink().toString() << endl;
        QDesktopServices::openUrl(m_dropbox->authorizeLink());

        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(NULL, tr("DropBox Authrization"), tr("Please confirm to authorize this application"));
        m_dropbox->requestAccessTokenAndWait();
}

    /**
     * @brief Connect a QDropbox to the Dropbox service
     * @param d QDropbox object to be connected
     * @param m Authentication Method
     * @return <code>true</code> on success
     */
bool DropBoxClient::connectDropbox(QDropbox::OAuthMethod m)
{
        QFile tokenFile("tokens");

        if(tokenFile.exists()) // reuse old tokens
        {
            if(tokenFile.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                QTextStream instream(&tokenFile);
                QString token = instream.readLine().trimmed();
                QString secret = instream.readLine().trimmed();
                if(!token.isEmpty() && !secret.isEmpty())
                {
                    m_dropbox->setToken(token);
                    m_dropbox->setTokenSecret(secret);
                    tokenFile.close();
                    getList("dropbox/Photos");
                    return true;
                }
            }
            tokenFile.close();
        }

        // acquire new token
        if(!m_dropbox->requestTokenAndWait())
        {
            qCritical() << "error on token request";
            return false;
        }

        m_dropbox->setAuthMethod(m);
        if(!m_dropbox->requestAccessTokenAndWait())
        {
            int i = 0;
            for(;i<3; ++i) // we try three times
            {
                if(m_dropbox->error() != QDropbox::TokenExpired)
                    break;
                authorizeApplication();
            }

           if(i>3)
           {
               qCritical() <<  "too many tries for authentication";
               return false;
           }

            if(m_dropbox->error() != QDropbox::NoError)
            {
               qCritical() << "Error: " << m_dropbox->error() << " - " << m_dropbox->errorString() << endl;
               return false;
            }
        }

        if(!tokenFile.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text))
            return true;

        QTextStream outstream(&tokenFile);
        outstream << m_dropbox->token() << endl;
        outstream << m_dropbox->tokenSecret() << endl;
        tokenFile.close();
        getList("dropbox/Photos");
        return true;
}



DropBoxClient::~DropBoxClient()
{
    delete m_dropbox;
}


QString DropBoxClient::downloadFile(const QString &filename)
{
    QProgressDialog progress("Downloading file...", NULL, 0, 0, NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    QString fullFileName= filename;
    QString homeDir=QDir::homePath();
    homeDir.append("/Pictures");
    fullFileName.prepend("dropbox");
    QDropboxFile dropboxFile(fullFileName,m_dropbox);
    dropboxFile.open(QIODevice::ReadOnly);

    QByteArray fileBuff;
    fileBuff=dropboxFile.readAll();
    QString outputFileName=dropboxFile.filename();
    dropboxFile.close();
    QDir dir(homeDir+"/dropbox/Photos");
    if(!dir.exists())
    {
        dir.mkpath(".");
    }
    outputFileName.prepend(homeDir+"/");
    QFile file(outputFileName);
    if(!file.open(QIODevice::WriteOnly))
        return;

    file.write(fileBuff);
    file.close();
    return outputFileName;

}


void DropBoxClient::uploadFile(const QString &filename)
{
    QProgressDialog progress("Uploading file...", NULL, 0, 0, NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    QString fullFileName="dropbox/Photos";
    int index=filename.lastIndexOf("/Encoded");
    QString shortName=filename.right(filename.length()-index);
    fullFileName.append(shortName);
    QDropboxFile dropboxFile(fullFileName,m_dropbox);
    QFile file(filename);
    QByteArray fileBuff;
    if(file.open(QIODevice::ReadOnly))
    {
        fileBuff=file.readAll();
        file.close();
    }

    if(dropboxFile.open(QIODevice::WriteOnly))
    {
        dropboxFile.write(fileBuff);
        dropboxFile.close();
    }
    getList("dropbox/Photos");
}

bool DropBoxClient::isDir(const QString &str) const
{
    return this->isDirHash[str];
}


void DropBoxClient::changeDir(const QString &dir)
{
    QString cur;
    int index, flg;

    cur = getCurrentPath().trimmed();
    index = cur.lastIndexOf('/');

    if (dir == tr(".."))	/* chanage to parrent */
    {
        if (index > 0)
        {
            cur = cur.left(index);
            setCurrentPath(cur);
            flg = 1;

        }
        else
        {
            cur = "/";
            setCurrentPath(cur);
            flg = 0;

        }
    }
    else
    {
        cur = cur.append("/").append(dir);
        setCurrentPath(cur);
        flg = 2;		/* è¿”å›žåˆ°æ›´æ·±ä¸€å±‚ç›®å½• */

    }

}

void DropBoxClient::setCurrentPath(QString &path)
{
    curPath = path;
}




QString DropBoxClient::getUrlStr(const QUrl &url)
{
    QString str;
    int index;

    str = url.toString().trimmed();
    index = str.lastIndexOf("/");
    if (index >= 0 && index < str.length())
    {
        str = str.left(index);
    }

    return str;
}

void DropBoxClient::getList(const QString path)
{
    curPathInfo=m_dropbox->requestMetadataAndWait(path);
    emit cmdAddToList(curPathInfo);
}
