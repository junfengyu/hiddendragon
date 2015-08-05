#ifndef CONFIG_H
#define CONFIG_H


#include <QtCore>
#include <QDomDocument>
#include <QDomElement>



class Config
{
    private:
        bool m_isLoaded;
        QString m_filename;
        QString m_filepath;
        QMap<QString, QString> m_valueMap;

    public:
        Config();
        Config(QString filePath, QString filename);
        Config(const Config& config);
        ~Config();

        QString filename() const;
        QString filepath() const;
        QMap<QString, QString> values() const;

        void set(const QString name, const QString value);
        QString get(const QString name) const;
        bool contains(const QString name) const;

        void save();
        bool isLoaded() const;

        Config& operator=(const Config& config);

    private:
        bool load();

        //public:
        //   static QString path(){ return "config/"; };
};




#endif // CONFIG_H
