#include "moduleexception.h"



    ModuleException::ModuleException(const QString& message) throw()
        : HiddenDragonException(message) {
    }

    ModuleException::ModuleException(const QString& message,
                                     const QString& details) throw()
                                         : HiddenDragonException(message, details){
    }

    ModuleException::ModuleException(const ModuleException& exception) throw()
        : HiddenDragonException(exception){
    }

    ModuleException::~ModuleException() throw(){}

