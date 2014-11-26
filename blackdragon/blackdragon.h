#ifndef BLACKDRAGON_H
#define BLACKDRAGON_H

#include <QByteArray>
#include <QPointer>


#include <stdio.h>
#include <stdlib.h>
#include "encodeddata.h"

#include "blackdragon/defs.h"
#include "blackdragon/bd.h"
class BlackDragon{
    private:
        static char* longToByteArray(ulonglong value);
        static void XOR(QByteArray source, QByteArray &target, QByteArray &key, int length);
    public:
        static QPointer<EncodedData> encode(QString key, QPointer<EncodedData> msg);
        static QByteArray decode(QString key, QByteArray &encodedData);
};

#endif // BLACKDRAGON_H
