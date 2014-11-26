#ifndef FORMATMODULEINTERFACE_H_
#define FORMATMODULEINTERFACE_H_

#include "moduleinterface.h"
#include "image.h"



    //! Common interface for image format plug-ins
    class FormatModuleInterface : public ModuleInterface
    {
    public:
        virtual QWidget* encodeWidget() const = 0;
        virtual bool isEncodeWidgetReady() const = 0;
        virtual QWidget* decodeWidget() const = 0;
        virtual bool isDecodeWidgetReady() const = 0;

        virtual QPointer<Image> encodeImage(QPointer<Image>, bool=false) = 0;
        virtual QPointer<Image> decodeImage(QPointer<Image>, bool=false) = 0;
    signals:
        void optionChanged();
    };



Q_DECLARE_INTERFACE(FormatModuleInterface,
                    "HiddenDragon.FormatModuleInterface/1.0");

#endif
