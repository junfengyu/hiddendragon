#include "encodedialog.h"
#include "controller.h"
#include "blackdragon/blackdragon.h"


EncodeDialog::EncodeDialog(QWidget* parent)
        : OptionDialog(parent)
{
        this->setObjectName("EncodeDialog");
        m_logger->setClassName("EncodeDialog");

        this->setWindowTitle("Encode message");
        this->setWindowIcon(QPixmap(QString::fromUtf8(":/icons/img/encode.png")));

        okButton->setText("Encode");
        iconLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/img/encode.png")));
        msgLabel->setText("Write a message or select a file to hide into your image");
      

        connect(fileButton, SIGNAL(pressed()), this, SLOT(selectFile()));
}

void EncodeDialog::showEvent(QShowEvent* event)
{
        if(!event->spontaneous())
        {
            m_logger->debug("set default output dir: " + Controller::config.get("output"));
            destinationLineEdit->setText(Controller::config.get("output"));
            msgTextEdit->setText("");
            cryptoPwd1LineEdit->setText("");
            cryptoPwd2LineEdit->setText("");
            checkCryptoPassword();
            imageFormatChanged(imageFormatComboBox->currentText());
            if(m_img != NULL)
                this->setWindowTitle("Encode message : " + m_img->filePath());
         
            removeFile();
        }
    }

void EncodeDialog::dragEnterEvent(QDragEnterEvent *event)
{
        if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
        else
            m_logger->debug(event->mimeData()->formats().join(";"));
}

void EncodeDialog::dropEvent(QDropEvent *event)
{
        if(event->mimeData()->hasUrls())
        {
            QList<QUrl> urls = event->mimeData()->urls();
            for(int i=0; i<urls.size(); i++)
            {
                if(urls.at(i).toLocalFile().length()>0)
                    addFile(urls.at(i).toLocalFile());
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"),
                                 "The dropped object doesn't have any url.");
        }

}

   
void EncodeDialog::ok()
{

        if(!isDialogReady())
            return;

      
        if(msgTextEdit->toPlainText().length()==0 && m_filePath == "")
        {
            QMessageBox::critical(this, tr("HiddenDragon Warning"),
                                  "You must provide a message or select a file to hide into your image.");
            return;
        } else if (m_filePath != "" && !QFile::exists(m_filePath)) {
            QMessageBox::critical(this, tr("HiddenDragon Warning"),
                                  "Selected file doesn't exist!");
            return;
        }

        QString output = destinationLineEdit->text();
        if(!QFile::exists(output))
        {
            QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                 "Destination folder doesn't exists!");
            return;
        }




        setCursor(Qt::WaitCursor);

        if (defaultCheckBox->checkState()!=Qt::Unchecked)
        {
            Controller::config.set("output", output);
            Controller::config.save();
        }

        CryptoModuleInterface* module;
      
            bool compress= false;
            QPointer<EncodedData> data;
            if (m_filePath != "" && QFile::exists(m_filePath))
            {
                m_logger->debug("File to hide: " + m_filePath);
                QFile file(m_filePath);
                data = new EncodedData(file, compress);
            }
            else
            {
                m_logger->debug("Message to hide: " + msgTextEdit->toPlainText());
                data = new EncodedData(msgTextEdit->toPlainText(), currentCharset(), compress);
            }

			data = BlackDragon::encode(cryptoPwd2LineEdit->text(), data);

     			   
            if(output+"/"+m_img->shortName() == m_img->filePath())
            {
                QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     "Cannot save new file into source directory, please select an other destination!");
                return;
            }
            else if( QFile::exists(output+"/"+m_img->shortName()) )
            {
                setCursor(Qt::ArrowCursor);
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText("Destination file already exists !");
                msgBox.setDetailedText(output+"/"+m_img->shortName());
                msgBox.setInformativeText("Do you really want to override it ?");
                msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);
                if(msgBox.exec()==QMessageBox::Cancel)
                    return;
            }

            m_img->setData(data);

            QString errorMsg;

			if(m_imageFormat==QString("BMP"))
			{
				if(!m_img->EncoderHandler("bmp", output))	
				{
                	errorMsg += "- An error occured during the encoding process.\n";
            	}
			}else if(m_imageFormat==QString("JPEG"))
			{
				if(!m_img->EncoderHandler("jpeg", output))	
				{
                	errorMsg += "- An error occured during the encoding process.\n";
            	}
			}

			setCursor(Qt::ArrowCursor);
            emit encodedImage(m_img->outputFileName());
            this->accept();

    
        setCursor(Qt::ArrowCursor);
    }


