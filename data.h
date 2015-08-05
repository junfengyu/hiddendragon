#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QByteArray>
#include "logger.h"


class Data : public QObject
    {
    public:
        //! Data format
        enum DataFormat{
            BYTES=0, /*!< raw data */
            UINT32=1, /*!< unsigned int 32bit */
            UTF8=2, /*!< String using UF8. */
            LATIN1=3, /*!< String using latin */
            ASCII=4, /*!< String using ascii table */
            FILE=5, /*!< File */
            F_UNDEF=7 }; /*!< undefined format (default) */

    private:
        //! Logger
        QPointer<Logger> m_logger;

        //! Current data format
        DataFormat m_format;
        //! Specific name (ex: filename)
        QString m_name;
        //! Information data
        QByteArray m_data;

    public:
        Data(const DataFormat);
        Data(const QByteArray&, const DataFormat=F_UNDEF);
        Data(const DataFormat, const QByteArray&, const QString="");
        ~Data();

        DataFormat format() const;
        QString name() const;
        QByteArray data() const;

        void setFormat(const DataFormat);
        void setName(const QString);
        void setData(const QByteArray&);

        QByteArray toByteArray() const;
    };



#endif
