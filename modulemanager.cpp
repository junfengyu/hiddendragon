

#include "modulemanager.h"

    Logger ModuleManager::m_logger("ModuleManager");
    QList<ModuleInterface*> ModuleManager::m_modulesList;
    QMap< ModuleManager::Category, QMap<QString, ModuleInterface*> > 
    ModuleManager::m_modulesMap;
    QMap<QString, ModuleInterface*> ModuleManager::m_emptyMap;

    QList<ModuleInterface*>& ModuleManager::getList()
    {
        return m_modulesList;
    }

    QMap<QString, ModuleInterface*>& 
    ModuleManager::get(Category cat)
    {
        if(m_modulesMap.contains(cat))
            return m_modulesMap[cat];

        return m_emptyMap;
    }

    ModuleInterface* 
    ModuleManager::get(Category cat, QString type)
    {
        if(m_modulesMap.contains(cat) && m_modulesMap[cat].contains(type))
            return m_modulesMap[cat][type];

        return NULL;
    }

    ModuleManager::Category ModuleManager::getCategory(ModuleInterface* module)
    {
        QMap<Category, QMap<QString, ModuleInterface*> >::const_iterator moduleMap = m_modulesMap.constBegin();
        while( moduleMap != m_modulesMap.constEnd() )
        {
            QMap<QString,ModuleInterface*>::const_iterator i = moduleMap.value().constBegin();
            while( i != moduleMap.value().constEnd() )
            {
                if( i.value() == module )
                    return moduleMap.key();
                ++i;
            }
            ++moduleMap;
        }

        return C_UNDEF;
    }

    int
    ModuleManager::count()
    {
        return m_modulesList.count();
    }

    int 
    ModuleManager::count(Category cat)
    {
        if(m_modulesMap.contains(cat))
            return m_modulesMap[cat].count();
        return 0;
    }

    void ModuleManager::load()
    {
        /* Load Module */
        QDir pluginsDir = QDir(qApp->applicationDirPath());
        pluginsDir.cd("modules");

        /* Load all modules in modules directory */
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            if(!QLibrary::isLibrary(fileName))
            {
                continue;
            }
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if( plugin )
            {
                FormatModuleInterface* moduleF = qobject_cast<FormatModuleInterface*>(plugin);
                if( moduleF )
                {
                    m_modulesMap[IMAGEFORMAT][moduleF->typeSupported()] = moduleF;
                    m_modulesList << moduleF;
                }
                else
                {
                    plugin = loader.instance();
                    CryptoModuleInterface* moduleC = qobject_cast<CryptoModuleInterface*>(plugin);
                    if( moduleC )
                    {
                        m_modulesMap[CRYPTO][moduleC->typeSupported()] = moduleC;
                        m_modulesList << moduleC;
                    }
                }
            } else {
                m_logger.warning(loader.errorString());
            }
        }
    }


