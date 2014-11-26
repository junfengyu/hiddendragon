

#ifndef CRYPTOMODULEINTERFACE_H_
#define CRYPTOMODULEINTERFACE_H_

#include "moduleinterface.h"
#include "encodeddata.h"

    //! Common interface for cryptography plug-ins
    class CryptoModuleInterface : public ModuleInterface
    {
    public:
        //! Encrypte data using a specific key
        virtual QPointer<EncodedData> encode(QString key, QPointer<EncodedData> msg) = 0;
        //! Decrypte data using a specific key
        virtual QPointer<EncodedData> decode(QString key, QPointer<EncodedData> data) = 0;
    };


Q_DECLARE_INTERFACE(CryptoModuleInterface,
                    "HiddenDragon.CryptoModuleInterface/1.0");

#endif
