#include "config.h"


Config::Config()
{
        m_isLoaded = false;
        m_filename = "se-noname.conf";
        m_filepath = "/tmp/";
}

Config::Config(QString filePath, QString filename)
{
        m_filename = filename+".conf";
        m_filepath = filePath;

        m_isLoaded = load();
}

Config::~Config()
{

}

QString Config::filename() const
{
        return m_filename;
}

QString Config::filepath() const
{
        return m_filepath;
}

QMap<QString, QString> Config::values() const
{
        return m_valueMap;
}

Config::Config(const Config& config)
{
        m_filename = config.filename();
        m_valueMap = config.values();
}

bool Config::isLoaded() const
{
        return m_isLoaded;
}

bool Config::contains(QString name) const
{
        return m_valueMap.contains(name);
}

void Config::set(QString name, QString value)
{
        m_valueMap[name] = value;
}

QString Config::get(QString name) const
{
        if (m_valueMap.contains(name))
            return m_valueMap.value(name);
        else
            return QString();
}

void Config::save()
{
        QDomDocument doc( "HiddenDragon" );
        QDomElement root = doc.createElement("configuration");
        doc.appendChild(root);

        QMap<QString, QString>::const_iterator i = m_valueMap.constBegin();
        while (i != m_valueMap.constEnd()) {
            QDomElement conf = doc.createElement(i.key());
            conf.appendChild(doc.createTextNode(i.value()));
            root.appendChild(conf);
            ++i;
        }

        /* open the output file */
        QFile file(m_filepath+m_filename);
        if( !file.open( QIODevice::WriteOnly ) )
        {
            qWarning() << "Can't save configuration file to " << m_filepath+m_filename;
            return;
        }

        /* write data to the output file */
        QTextStream ts( &file );
        /* Profile serialize datas */
        ts << doc.toString();
        file.close();
}

bool Config::load()
{
        QDomDocument doc( "HiddenDragon" );

        QFile file(m_filepath+m_filename);
        if(file.open( QIODevice::ReadOnly ) )
        {
            /* load file content to xml document */
            if(doc.setContent( &file ) )
            {
                file.close();
                QDomElement root = doc.documentElement();

                QDomNode n = root.firstChild();

                while(!n.isNull())
                {
                    QDomElement e = n.toElement();
                    if( !e.isNull() )
                    {
                        m_valueMap[e.tagName()] = e.text();
                    }
                    n = n.nextSibling();
                }
                return true;
            }
            else
                file.close();
        }
        else
            qWarning() << "Can't open configuration file: " << m_filepath+m_filename;

        return false;
}

Config& Config::operator=(const Config& config)
{
        m_filename = config.filename();
        m_filepath = config.filepath();
        m_valueMap = config.values();
        return *this;
}


