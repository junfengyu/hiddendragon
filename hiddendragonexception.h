#ifndef HIDDENDRAGONEXCEPTION_H
#define HIDDENDRAGONEXCEPTION_H


#include <QtCore>



    //! Internal exception only used by HiddenDragonFramework
class HiddenDragonException : public QtConcurrent::Exception
{
    protected:
        QString m_message;
        QString m_details;

    public:
        HiddenDragonException(const QString& message) throw();
        HiddenDragonException(const QString& message, const QString& details) throw();

        HiddenDragonException(const HiddenDragonException& exception) throw();
        ~HiddenDragonException() throw();

        QString message() const;
        QString details() const;

        void raise() const{ throw *this; }
        Exception *clone() const{ return new HiddenDragonException(*this); }
};



#endif
