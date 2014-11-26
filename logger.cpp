#include "logger.h"

#include <QTime>
#include <QDebug>



QPointer<QFile> Logger::m_file;
int Logger::m_nbInstance = 0;
QString m_fileName = "application.log";
Logger::LogLevel Logger::m_level = DEBUG_LEVEL;

Logger::Logger(QObject* parent) : QObject(parent)
{
        if (parent != 0)
        {
            m_className = parent->objectName();
        }
        m_nbInstance++;
}

Logger::Logger(QString className)
{
        m_className = className;
        m_nbInstance++;
}

void Logger::init()
{
        if (m_file.isNull()) {
            m_file = new QFile(m_fileName);
            if (!m_file->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                qWarning() << "> Cannot open file " << m_fileName << " in write mode!";
            }
        }
}

Logger::~Logger() {
        m_nbInstance--;
        if (m_nbInstance <= 0 && !m_file.isNull()) {
            m_file->close();
            delete m_file;
        }
}

QString Logger::className() const
{
        return m_className;
}

void Logger::setClassName(const QString& name)
{
        m_className = name;
}

void Logger::write(QString priority, QString text)
{
        init();
        QString value = priority + "[" + m_className + "][" + QTime::currentTime().toString("hh:mm:ss") + "] " + text;
        qDebug() << value;
        m_file->write(QString(value + "\n").toLatin1());
        m_file->flush();
}

void Logger::debug(QString text)
{
        if (m_level <= DEBUG_LEVEL)
            write("DEBUG", text);
}

void Logger::info(QString text)
{
        if (m_level <= INFO_LEVEL)
            write("INFO", text);
}

void Logger::warning(QString text)
{
        if (m_level <= WARNING_LEVEL)
            write("WARNING", text);
}

void Logger::error(QString text)
{
        if (m_level <= ERROR_LEVEL)
            write("ERROR", text);
}

void Logger::setFileName(const QString& name)
{
        m_fileName = name;
}

void Logger::setLevel(Logger::LogLevel level)
{
        m_level = level;
}

void Logger::setLevel(const QString& level)
{
        if (level == "INFO")
            m_level = INFO_LEVEL;
        else if (level == "WARNING")
            m_level = WARNING_LEVEL;
        else if (level == "ERROR")
            m_level = ERROR_LEVEL;
        else
            m_level = DEBUG_LEVEL;
}



