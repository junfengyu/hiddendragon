#ifndef SPLITFILE_H
#define SPLITFILE_H

#include <QObject>
#include <QString>
class SplitFile : public QObject
{
    Q_OBJECT
public:
    explicit SplitFile(QString fileName, int pieces, QObject *parent = 0);
    ~SplitFile();
    bool isFileExist(QString filename);
    int doSplit();
    int doJoin();
private:
    QString m_fileName,m_outputFileName;
    int m_pieces;
signals:

public slots:

};

#endif // SPLITFILE_H
