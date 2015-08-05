
#include <math.h>

#include "encodeddata.h"
#include "hiddendragonexception.h"


EncodedData::EncodedData(Data::DataFormat format, bool compress)
        : _compressed(compress)
{
        _swap = 2;
        _andOperator = andOperator(_swap);
        _arrayCount = 0;
        _bitCount = 0;
        _car = 0;
        _buffer.clear();
        _data = new Data(format);
        _partialData = true;
}


EncodedData::EncodedData(const QByteArray bytes, Data::DataFormat format, bool compress)
        : _compressed(compress)
{
        _swap = 2;
        _andOperator = andOperator(_swap);
        _arrayCount = 0;
        _bitCount = 0;
        _car = 0;
        if(format == Data::F_UNDEF) {
            _data = new Data(bytes, format);
        } else {
            _data = new Data(format, bytes);
            if (compress)
                _buffer = qCompress(_data->toByteArray(), 9);
            else
                _buffer = _data->toByteArray();
        }

        _partialData = false;
}

EncodedData::EncodedData(const quint32 value, bool compress)
        : _compressed(compress)
{
        _swap = 2;
        _andOperator = andOperator(_swap);
        _arrayCount = 0;
        _bitCount = 0;
        _car = 0;

        for(int i=0; i<=24; i+=8)
        {
            uchar car = (char)((value >> i)&255);
            _buffer.append(car);
        }

        _data = new Data(Data::UINT32, _buffer);
        if (compress)
            _buffer = qCompress(_data->toByteArray(), 9);
        else
            _buffer = _data->toByteArray();

        _partialData = false;
}


EncodedData::EncodedData(const QString& str, Data::DataFormat format, bool compress)
        : _compressed(compress)
{
        _swap = 2;
        _andOperator = andOperator(_swap);
        _arrayCount = 0;
        _bitCount = 0;
        _car = 0;

        if(format == Data::UTF8)
            _data = new Data(format, str.toUtf8());
        else if(format == Data::LATIN1)
            _data = new Data(format, str.toLatin1());
        else
            _data = new Data(format, str.toAscii());


        if (compress)
            _buffer = qCompress(_data->toByteArray(), 9);
        else
            _buffer = _data->toByteArray();

        _partialData = false;
}

EncodedData::EncodedData(QFile& file, bool compress)
        : _compressed(compress)
{
        _swap = 2;
        _andOperator = andOperator(_swap);
        _arrayCount = 0;
        _bitCount = 0;
        _car = 0;
        file.open(QFile::ReadOnly);
        _buffer = file.readAll();
        file.close();

        QString filePath = file.fileName();
        _data = new Data(Data::FILE, _buffer, QDir::fromNativeSeparators(filePath).section("/", -1, -1));
        if (compress)
            _buffer = qCompress(_data->toByteArray(), 9);
        else
            _buffer = _data->toByteArray();

        _partialData = false;
}

EncodedData::~EncodedData()
{
        if (!_data.isNull())
            delete _data;
}


void EncodedData::checkPartialData()
{
        if(_partialData)
        {
            if (_compressed)
                _data = new Data(qUncompress(_buffer), _data->format());
            else
                _data = new Data(_buffer, _data->format());

            _partialData = false;
        }
}


Data::DataFormat EncodedData::format()
{
        checkPartialData();
        return _data->format();
}


bool EncodedData::isCompressed() const
{
        return _compressed;
}


void EncodedData::setCompressed(const bool compress, const bool force)
{
        if(force)
        {
            if(!_compressed && compress)
            {
                _buffer = qCompress(_buffer, 9);
            }
            else if(_compressed && !compress)
            {
                _buffer = qUncompress(_buffer);
            }
        }
        _compressed = compress;

}


QByteArray EncodedData::bytes()
{
        return _buffer;
}

quint32 EncodedData::size() const
{
        return _buffer.size();
}

void EncodedData::clear()
{
        _arrayCount = 0;
        _bitCount = 0;
        _car = 0;
        _buffer.clear();
        Data::DataFormat format = Data::F_UNDEF;
        if(!_data.isNull())
        {
            format = _data->format();
            delete _data;
        }
        _data = new Data(format);
}


void EncodedData::initialize(unsigned short int nbBits)
{
        _swap = nbBits;
        _andOperator = andOperator(_swap);
        _arrayCount = 0;
        _bitCount = 0;
        if(_buffer.size()>0)
            _car = _buffer.at(0);
}


bool EncodedData::hasNext()
{
        if(_buffer.size() == 0)
            return false;
        else
            return (_arrayCount < (unsigned int)_buffer.size());
}


int EncodedData::read()
{
        int val = 0;
        if(hasNext())
        {
            int bitsLeft = 8 - _bitCount;
            if(bitsLeft < _swap)
            {
                val = _car & andOperator(bitsLeft);

                int bitsRemaining = _swap - bitsLeft;
                if(_arrayCount < (unsigned int)_buffer.size() - 1)
                {
                    _car = _buffer.at(_arrayCount + 1);
                    val += ((_car & andOperator(bitsRemaining)) << bitsLeft);

                    _car = _car >> bitsRemaining;
                    _bitCount = bitsRemaining;
                }
                _arrayCount++;
            }
            else
            {
                val = _car & _andOperator;

                if(_bitCount + _swap >= 8)
                {
                    _bitCount = 0;
                    if(_arrayCount < (unsigned int)_buffer.size()-1)
                    {
                        _car = _buffer.at( _arrayCount+1 );
                    }
                    _arrayCount++;
                }
                else
                {
                    _car = _car >> _swap;
                    _bitCount += _swap;
                }
            }
        }
        return val;
}

void EncodedData::append(int val)
{
        _partialData = true;

        int bitsLeft = 8 - _bitCount;
        int tempVal;
        if(bitsLeft < _swap)
        {
            tempVal = val;
            val = val & andOperator(bitsLeft);
        }

        _car += (val << _bitCount);
        if(_bitCount + _swap >= 8)
        {
            _buffer.append(_car);
            _arrayCount++;
            _bitCount = 0;
            _car = 0;
        }
        else
        {
            _bitCount += _swap;
        }

        if (bitsLeft < _swap)
        {
            int bitsRemaining = _swap - bitsLeft;
            val = tempVal >> bitsLeft;
            _car += val;
            _bitCount += bitsRemaining;
        }
}


quint32 EncodedData::toUInt32()
{
        checkPartialData();

        quint32 value = 0;
        QByteArray bytes = _data->data();
        for(int i=0; i<bytes.size() && i<4; i++)
            value += ((uchar)bytes.at(i))<<(i*8);

        return value;
}


QString EncodedData::toString(Data::DataFormat format)
{
        checkPartialData();
        Data::DataFormat dataFormat = _data->format();
        if(format!=Data::F_UNDEF)
            dataFormat = format;

        int dataSize = size();
        QByteArray bytes = _data->data();
        if (dataSize>0 && bytes.size()==0)
        {
            throw HiddenDragonException("Cannot uncompress data",
                                     "check other options and make sure the given image include a compressed message.");
        }
        if(dataFormat == Data::UTF8)
            return QString::fromUtf8(bytes.data());
        else if(dataFormat == Data::LATIN1)
            return QString::fromLatin1(bytes.data());
        else if(dataFormat == Data::ASCII)
            return QString::fromAscii(bytes.data());
        else if(dataFormat == Data::FILE)
            return _data->name();

        return "unsupported format(" + QString::number(dataFormat) + ") for string conversion.";
}


QPointer<Data> EncodedData::toData()
{
        checkPartialData();
        return _data;
}

unsigned short int EncodedData::andOperator(unsigned short int nbBits)
{
        return (int)pow(2, nbBits)-1;
}


