
#ifndef MODULEINTERFACE_H_
#define MODULEINTERFACE_H_

#include <QObject>
#include "moduleexception.h"

    //! Common interface for all HiddenDragon's plug-in.
    class ModuleInterface : public QObject
    {
    public:
        //! Plug-in name (ex: FooBar)
        virtual QString name() const = 0;
        //! Plug-in version (ex: 1.2.0)
        virtual QString version() const = 0;
        //! Type supported (ex: BMP, JPEG, AES128, etc.)
        virtual QString typeSupported() const = 0;
	//! Current status of the module (ex: KO|qca2-plugin-ossl missing)
        virtual QString status() const = 0;
    };



Q_DECLARE_INTERFACE(ModuleInterface,
                    "HiddenDragon.ModuleInterface/1.0");

#endif
