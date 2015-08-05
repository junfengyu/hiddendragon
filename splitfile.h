#ifndef SPLITFILE_H
#define SPLITFILE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
static const int BUFFER_SIZE= 8096;
static const int NAME_SIZE=2048;
class SplitFile : public QObject
{
    Q_OBJECT
public:
    explicit SplitFile(const QString fileName, const int pieces, QObject *parent = 0);
    SplitFile(const QStringList &splitFilesName, QObject *parent = 0);
    ~SplitFile();
    QStringList &getSplitedFilesList();
    bool isFileExist(QString filename);
    bool doSplit();
    bool doJoin();
    QString &getOriginalFileName();
private:
    QString m_fileName;
    QStringList m_SplitFiles;
    int m_pieces;
    QMap <int,QString> m_filesMap;
private:
    void guessOrder();
signals:

public slots:

};


#endif // SPLITFILE_H
