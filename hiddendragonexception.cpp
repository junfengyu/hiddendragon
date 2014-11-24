#include "hiddendragonexception.h"
#include <QString>


    HiddenDragonException::HiddenDragonException(const QString& message) throw()
        : Exception() {
        m_message = message;
        m_details = "";
    }

    HiddenDragonException::HiddenDragonException(const QString& message,
                                           const QString& details) throw()
                                               : Exception(){
        m_message = message;
        m_details = details;
    }

    HiddenDragonException::HiddenDragonException(const HiddenDragonException& exception) throw()
        : Exception(){
        m_message = exception.message();
        m_details = exception.details();
    }

    HiddenDragonException::~HiddenDragonException() throw(){}

    QString HiddenDragonException::message() const{
        return m_message;
    }

    QString HiddenDragonException::details() const{
        return m_details;
    }


