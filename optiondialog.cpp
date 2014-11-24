#include "optiondialog.h"
#include "controller.h"
#include "hiddendragonexception.h"

OptionDialog::OptionDialog(QWidget* parent)
        : QDialog(parent)
{
        setupUi(this);
        m_logger = new Logger(this);

      
        m_checkSize = true;
        m_originalHeight = minimumHeight();
        loadImageFormats();
        loadEncryptionTypes();

        setupDialog();
        connectSignals();
}

OptionDialog::~OptionDialog()
{
    if(!m_img.isNull())
        delete(m_img);
    delete m_logger;
}

void OptionDialog::setImage(QPointer<Image> img)
{
    if (m_img.isNull())
        delete m_img;
        // get an image copy
    m_img = new Image(*img);
}

void OptionDialog::selectFolder()
{
        setCursor(Qt::WaitCursor);
        QFileDialog dialog(this, tr("Save image to"));
        dialog.setViewMode(QFileDialog::List);
        dialog.setFileMode(QFileDialog::DirectoryOnly);
        dialog.setDirectory(Controller::config.get("output"));
        if(dialog.exec())
        {
            QStringList fileNames = dialog.selectedFiles();
            if(fileNames.size()>0)
            {
                if(QFile::exists(fileNames.at(0)))
                    destinationLineEdit->setText(fileNames.at(0));
                else
                    QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         fileNames.at(0)+"\n"+
                                         "Selected folder doesn't exists.\n"+
                                         "Please select an other destination!");
            }
        }
        setCursor(Qt::ArrowCursor);
}

void OptionDialog::selectFile()
{
        bool save = this->objectName() == "DecodeDialog";
        setCursor(Qt::WaitCursor);
        QFileDialog dialog(this, (save)?tr("Select destination"):tr("Select file to hide"));
        dialog.setViewMode(QFileDialog::List);
        if (save)
            dialog.setFileMode(QFileDialog::DirectoryOnly);
        else
            dialog.setFileMode(QFileDialog::ExistingFiles);
        dialog.setDirectory(Controller::config.get("output"));
        if(dialog.exec())
        {
            QStringList fileNames = dialog.selectedFiles();
            if(fileNames.size()>0)
            {
                if (save)
                {
                	QString hiddenFileName;
                    QFile file(fileNames.at(0) + "/" + m_filePath);
                    file.open(QFile::WriteOnly);
                    file.write(m_img->data()->toData()->data());
                    file.close();

					

					
                    QMessageBox::information(this, tr("HiddenDragon Information"),
                                             "Decoded file has been saved to:\n" + file.fileName() + ".");
                }
                else
                    addFile(fileNames.at(0));
            }
        }
        setCursor(Qt::ArrowCursor);
}

void OptionDialog::addFile(const QString& filename)
{
        if(QFile::exists(filename))
        {
            QFile file(filename);
            if (file.open(QFile::ReadOnly))
            {
                file.close();
                m_filePath = filename;
                fileNameLabel->setText(QDir::fromNativeSeparators(m_filePath).section("/", -1, -1));
                msgTextEdit->setDisabled(true);
                fileRemoveButton->setEnabled(true);
                msgTextEdit->setText("");
              
            }
            else
                QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     filename+"\n"+
                                     "Selected file cannot be read!\n");
        }
        else
            QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                 filename+"\n"+
                                 "Selected file doesn't exists.\n");
}

void OptionDialog::removeFile()
{
    m_filePath = "";
    fileNameLabel->setText("no file selected");
    fileRemoveButton->setEnabled(false);
    msgTextEdit->setEnabled(true);
   
}

void OptionDialog::rejected()
{
}

bool OptionDialog::isDialogReady()
{
        if(m_img==NULL)
        {
            QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                 "No image has been selected, this is probably an internal error.\n please report it to developpers.");
            return false;
        }

        if(!checkCryptoPassword())
            {
                QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     "Key for encryption process have to be valid ! (match and have more than 5 characters)");
                return false;
            }

    /*
        if(m_checkSize && dataSizeRemaining() < 0)
        {
            QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                 "Selected image doesn't have enough space to encode your message!");
            return false;
        }
        */
        return true;
    }


/*
    CryptoModuleInterface* OptionDialog::currentCryptoModule()
    {
        QString type = cryptoComboBox->currentText();
        return (CryptoModuleInterface*)ModuleManager::get(ModuleManager::CRYPTO, type);
    }
    */ //!!!!!!!!!!!! ethan

    Data::DataFormat OptionDialog::currentCharset()
    {
        Data::DataFormat charset = Data::UTF8;

        return charset;
    }

    void OptionDialog::imageFormatChanged(const QString& imageFormat)
    {
    	m_imageFormat = imageFormat;
        /*
        FormatModuleInterface* module = (FormatModuleInterface*) ModuleManager::get(ModuleManager::IMAGEFORMAT, imageFormat);
        if(module==NULL) {
            m_logger->warning("Cannot find module for " + imageFormat + " image format" );
            return;
        }

        QWidget* w = currentFormatWidget(module);

        if(w!=NULL)
        {
            if(m_optionWidget!=NULL)
                m_optionWidget->setVisible(false);
            m_optionWidget = w;
            //optionsVerticalLayout->insertWidget(0, m_optionWidget);
            m_optionWidget->setVisible(true);
        }
        else
            m_logger->warning("The widget for encoding format is NULL");

        */
    }

    bool OptionDialog::checkCryptoPassword()
    {
        QPalette palette;
        bool return_value = false;

        if (cryptoPwd2LineEdit->text().length() >= 6
            && cryptoPwd1LineEdit->text() == cryptoPwd2LineEdit->text())
        {
            palette.setColor(QPalette::Base, QColor(179, 211, 155)); // green
            return_value = true;
        }
        else if (cryptoPwd2LineEdit->text().length() > 0 && cryptoPwd2LineEdit->text().length() <= 6)
            palette.setColor(QPalette::Base, QColor(255, 204, 204)); // red

        cryptoPwd1LineEdit->setPalette(palette);
        cryptoPwd2LineEdit->setPalette(palette);

        return return_value;
    }


  

    void OptionDialog::cryptoPwd1TextEdited(QString)
    {
        checkCryptoPassword();
    }

    void OptionDialog::cryptoPwd2TextEdited(QString)
    {
        checkCryptoPassword();
    }




void OptionDialog::loadImageFormats()
{
     imageFormatComboBox->addItem(QString("BMP"));
        if(Controller::config.get("imageformat")==QString("BMP"))
        {
            imageFormatComboBox->setCurrentIndex(imageFormatComboBox->count()-1);
			m_imageFormat=QString("BMP");
        }
     imageFormatComboBox->addItem(QString("JPEG"));
        if(Controller::config.get("imageformat")==QString("JPEG"))
        {
            imageFormatComboBox->setCurrentIndex(imageFormatComboBox->count()-1);
			m_imageFormat=QString("JPEG");
        }

}

void OptionDialog::loadEncryptionTypes()
{
     cryptoComboBox->addItem(QString("BlackDragon"));
     if(Controller::config.get("encryption")==QString("BlackDragon"))
     {
        cryptoComboBox->setCurrentIndex(cryptoComboBox->count()-1);
     }

}



void OptionDialog::setupDialog()
{
        cryptoWidget->setVisible(true);
        imageFormatWidget->setEnabled(true);
        okButton->setEnabled(true);
    
}

void OptionDialog::connectSignals()
    {
        connect(okButton, SIGNAL(pressed()), this, SLOT(ok()));
        connect(this, SIGNAL(rejected()), this, SLOT(rejected()));
        connect(toolButton, SIGNAL(pressed()), this, SLOT(selectFolder()));
        
        connect(cryptoPwd1LineEdit, SIGNAL(textEdited(QString)),
                this, SLOT(cryptoPwd1TextEdited(QString)));
        connect(cryptoPwd2LineEdit, SIGNAL(textEdited(QString)),
                this, SLOT(cryptoPwd2TextEdited(QString)));
        connect(imageFormatComboBox, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(imageFormatChanged(const QString&)));

      
        connect(fileRemoveButton, SIGNAL(pressed()), this, SLOT(removeFile()));
    }
/*
    void OptionDialog::displayException(const QString& title,
                                        const HiddenDragonException e){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(title);
        msgBox.setText(e.message());
        msgBox.setDetailedText(e.details());
        msgBox.exec();
    }

*/
