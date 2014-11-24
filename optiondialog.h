
#ifndef OPTIONDIALOG_H_
#define OPTIONDIALOG_H_

#include <QtGui>
#include <image.h>
#include <config.h>
#include <logger.h>

#include "ui_optiondialog.h"


class OptionDialog : public QDialog, public Ui::OptionDialog
    {
        Q_OBJECT;

    protected:
        QPointer<Logger> m_logger;

        QPointer<Image> m_img;
        QString m_filePath;
		QString m_imageFormat;
    
        bool m_checkSize;
        int m_originalHeight;

    public:
        OptionDialog(QWidget* parent=0);
        ~OptionDialog();

        void setImage(QPointer<Image> img);

    protected:
        void setupDialog();
    
        void loadImageFormats();
        void loadEncryptionTypes();
        bool isDialogReady();
        bool checkCryptoPassword();
     //   FormatModuleInterface* currentFormatModule();
       // CryptoModuleInterface* currentCryptoModule();
        Data::DataFormat currentCharset();
      
        //void displayException(const QString& title, const HiddenDragonException e);
    
    private:
        void connectSignals();

        protected slots:
        virtual void ok() = 0;
        void rejected();
        void selectFolder();
        void selectFile();
        void addFile(const QString&);
        void removeFile();
        void imageFormatChanged(const QString&);
        void cryptoPwd1TextEdited(QString);
        void cryptoPwd2TextEdited(QString);
     
};

#endif
