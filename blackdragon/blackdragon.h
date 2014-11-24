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
    static unsigned char* longToByteArray(ulonglong value);
    static void XORulonglong(QByteArray source, QByteArray &target, QByteArray &key, int length);
public:
    static QPointer<EncodedData> encode(QString key, QPointer<EncodedData> msg);
    static QPointer<EncodedData> decode(QString key, QPointer<EncodedData> data);
};

#endif // BLACKDRAGON_H
