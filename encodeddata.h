#ifndef ENCODEDDATA_H
#define ENCODEDDATA_H


#include <QPointer>
#include <QString>
#include <QByteArray>
#include <QFile>

#include "data.h"


class EncodedData : public QObject
{
    private:
        //! Readable representation of data
        QPointer<Data> _data;
        //! Compiled representation of data
        QByteArray _buffer;
        //! And operator used for bit read
        unsigned short int _andOperator;
        //! Number of bit read at once
        unsigned short int _swap;
        //! Index of last char read on _buffer
        unsigned int _arrayCount;
        //! Current readed char
        char _car;
        //! Number of bit reads from current char
        unsigned short int _bitCount;
        //! Indicate that data must be compressed or not
        bool _compressed;
        //! Indicate if current buffer contains partial data (ex: use of append)
        bool _partialData;

    public:
        EncodedData(Data::DataFormat=Data::F_UNDEF, bool=false);
        EncodedData(const QByteArray, Data::DataFormat=Data::BYTES, bool=false);
        EncodedData(const quint32, bool=false);
        EncodedData(const QString&, Data::DataFormat=Data::UTF8, bool=true);
        EncodedData(QFile&, bool=true);
        ~EncodedData();

        void initialize(unsigned short int = 2);
        bool hasNext();
        int read();
        void append(int);

        Data::DataFormat format();
        quint32 size() const;
        QByteArray bytes();
        void clear();

        bool isCompressed() const;
        void setCompressed(const bool, const bool=false);

        QString toString(Data::DataFormat=Data::F_UNDEF);
        quint32 toUInt32();
        QPointer<Data> toData();

        static unsigned short int andOperator(unsigned short int);
    private:
        void checkPartialData();
};



#endif
