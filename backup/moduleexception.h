#ifndef MODULEEXCEPTION_H
#define MODULEEXCEPTION_H



#include <QtCore>
#include "HiddenDragonexception.h"

    //! Exception used by all HiddenDragon's plug-in
class ModuleException : public HiddenDragonException
    {
    public:
        ModuleException(const QString& message) throw();
        ModuleException(const QString& message, const QString& details) throw();

        ModuleException(const ModuleException& exception) throw();
        ~ModuleException() throw();
    };



#endif
