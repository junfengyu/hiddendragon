#include "splitfile.h"
#include <QFile>
#include <QList>
#include <QMap>

SplitFile::SplitFile(const QString fileName, const int pieces, QObject *parent) :
    QObject(parent)
{
    m_fileName=QString::null;
    m_pieces=0;
    if(isFileExist(fileName))
    {
        m_fileName=fileName;
        m_pieces=pieces;

    }

}


SplitFile::SplitFile(const QStringList &splitFilesName, QObject *parent = 0)
{

    m_SplitFiles=splitFilesName;

}

void SplitFile::guessOrder()
{
    QString number;
    int index=0;
    foreach(const QString &str, m_SplitFiles)
    {
        index=str.lastIndexOf(".");
        index++;
        index=str.length()-index;
        number=str.right(index);
        m_filesMap.insert(number.toInt(),str);
    }

}

bool SplitFile::isFileExist(QString filename)
{

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return false;
        file.close();
        return true;
}




SplitFile::~SplitFile()
{


}

QStringList &SplitFile::getSplitedFilesList()
{
    return m_SplitFiles;
}

bool SplitFile::doSplit()
{
    if(m_fileName==QString::null)
        return false;
    QString originalFileName=m_fileName;
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QByteArray fileBuffer = file.readAll();
    file.close();
    long fileLength = fileBuffer.length();
    QList<long>  splitedFilesList;
    long eachLength= fileLength/m_pieces;
    long lastFileLength=eachLength;
    if(fileLength%m_pieces!=0)
        lastFileLength=fileLength-(m_pieces-1)*eachLength;

    for (int i=0; i< m_pieces; i++)
    {
       QString splitedFileName=originalFileName;
       splitedFileName.append("."+QString::number(i));
       m_SplitFiles << splitedFileName;
       QFile outFile(splitedFileName);
       QByteArray eachPiece;
       if (!outFile.open(QIODevice::WriteOnly))
           return false;
       if(i!=(m_pieces-1)){
            eachPiece=fileBuffer.left(eachLength);
            fileBuffer.remove(0,eachLength);
       }
       else eachPiece=fileBuffer.right(lastFileLength);
       outFile.write(eachPiece);
       outFile.close();
    }

    return true;

}
QString &SplitFile::getOriginalFileName()
{
    return m_fileName;

}

bool SplitFile::doJoin()
{

    guessOrder();
    QString tmpFileName=m_filesMap.value(0);
    int index=tmpFileName.lastIndexOf(".");
    QString originalFileName=tmpFileName.left(index);
   // originalFileName.append(".out");
    QByteArray totalFileBuffer;
    QFile outFile(originalFileName);
    m_fileName=originalFileName;


    if (!outFile.open(QIODevice::WriteOnly))
        return false;
    foreach (int key, m_filesMap.keys())
    {
        QString splitedFileName=m_filesMap.value(key);
        QFile file(splitedFileName);
        if (!file.open(QIODevice::ReadOnly))
            return false;
        QByteArray fileBuffer = file.readAll();
        file.close();
        totalFileBuffer.append(fileBuffer);

    }
    outFile.write(totalFileBuffer);
    outFile.close();

}
