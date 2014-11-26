
#ifndef MODULEMANAGER_H_
#define MODULEMANAGER_H_

#include <QtCore>
#include <moduleinterface.h>
#include <formatmoduleinterface.h>
#include <cryptomoduleinterface.h>
#include <logger.h>



    class ModuleManager
    {
    public:
        enum Category{ IMAGEFORMAT, CRYPTO, C_UNDEF };

    private:
        static Logger m_logger;
        static QMap<Category, QMap<QString, ModuleInterface*> > m_modulesMap;
        static QMap<QString, ModuleInterface*> m_emptyMap;
        static QList<ModuleInterface*> m_modulesList;

    public:
        static QList<ModuleInterface*>& getList();
        static QMap<QString, ModuleInterface*>& get(Category cat);
        static ModuleInterface* get(Category cat, QString type);

        static Category getCategory(ModuleInterface*);

        static int count();
        static int count(Category cat);

        static void load();

    };


#endif
