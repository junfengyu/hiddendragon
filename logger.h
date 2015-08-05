#ifndef LOGGER_H
#define LOGGER_H

#include <QPointer>
#include <QObject>
#include <QFile>
#include <QString>



class Logger : public QObject
{
    public:
        enum LogLevel{ DEBUG_LEVEL=1, INFO_LEVEL=5, WARNING_LEVEL=10, ERROR_LEVEL=15 };

    private:
        static QPointer<QFile> m_file;
        static int m_nbInstance;
        static QString m_logFileName;
        static LogLevel m_level;
        QString m_className;

    public:
        Logger(QObject* parent = 0);
        Logger(QString className);
        ~Logger();

        QString className() const;
        void setClassName(const QString&);

        void debug(QString);
        void info(QString);
        void warning(QString);
        void error(QString);

        static void setFileName(const QString&);
        static void setLevel(Logger::LogLevel=DEBUG_LEVEL);
        static void setLevel(const QString&);

    private:
        void init();
        void write(QString priority, QString text);
};


#endif // LOGGER_H
