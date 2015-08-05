#include <QList>
#include "encodedialog.h"
#include "controller.h"
#include "blackdragon/blackdragon.h"
#include "splitfile.h"



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
     
        bool compress= false;
        QPointer<EncodedData> data;
        QList <QPointer<EncodedData> > dataList;
        if(!multiImageBox->isChecked())
        {
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
            QString  imgName=m_img->shortName();
            int index=imgName.lastIndexOf(".");
            QString imgFormat=imgName.right(imgName.length()-index);
            QString errorMsg;
            if(m_imageFormat==QString("BMP")){

                if(imgFormat!=".bmp")
                {
                    errorMsg+="Please choose the right Image Format!\n";
                }
                else if(!m_img->EncoderHandler("bmp", output))
                {
                    errorMsg += "- An error occured during the encoding process.\n";
						QMessageBox::critical(this, tr("HiddenDragon Error"), errorMsg);
						setCursor(Qt::ArrowCursor);
            			return;
                }
            }
            else if(m_imageFormat==QString("JPEG"))
            {
                    if(imgFormat!=".jpg")
                    {
                        errorMsg+="Please choose the right Image Format!\n";
                    }
                    else if(!m_img->EncoderHandler("jpeg", output))
                    {
                        errorMsg += "- An error occured during the encoding process.\n";
						QMessageBox::critical(this, tr("HiddenDragon Error"), errorMsg);
						setCursor(Qt::ArrowCursor);
            			return;
                    }
            }
            if(errorMsg.length()>0)
            {
                QMessageBox::critical(this, tr("HiddenDragon Error"), errorMsg);
            }
            setCursor(Qt::ArrowCursor);
            emit encodedImage(m_img->outputFileName());
            this->accept();
            setCursor(Qt::ArrowCursor);
            return;
        }else //multi image
        {
            if (m_filePath != "" && QFile::exists(m_filePath))
            {
                m_logger->debug("File to hide: " + m_filePath);
                int pieces=m_inputImageFullNameMap.count();
                SplitFile dataFile(m_filePath,pieces);
                dataFile.doSplit();
                QStringList splitedFiles=dataFile.getSplitedFilesList();

                foreach(const QString &eachFile, splitedFiles)
                {
                    QFile file(eachFile);
                    data = new EncodedData(file, compress);
                    dataList << data;
                    file.remove();
                }
            }
            else
            {
                QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     "please uncheck Multi Image box, or please select a file!");
                setCursor(Qt::ArrowCursor);
                this->accept();
                setCursor(Qt::ArrowCursor);
                return;

            }

            foreach(const QPointer<EncodedData> dataPtr, dataList )
            {
                dataPtr = BlackDragon::encode(cryptoPwd2LineEdit->text(), dataPtr);
                dataList.pop_front();
                dataList << dataPtr;
            }


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
           // QMap<QString, QPointer<Image> > m_inputImageFullNameMap;
            int count=0;
            QString errorMsg;
            QString  imgName, imgFormat;
            foreach(QPointer<Image> imagePtr, m_inputImageFullNameMap)
            {
                imagePtr->setData(dataList.at(count));
                count++;
                imgName=imagePtr->shortName();
                int index=imgName.lastIndexOf(".");
                imgFormat=imgName.right(imgName.length()-index);
                if(m_imageFormat==QString("BMP")){
                    if(imgFormat!=".bmp")
                    {
                        errorMsg+="Please choose the right Image Format!\n";
                    }
                    else if(!imagePtr->EncoderHandler("bmp", output))
                    {
                        errorMsg += "- An error occured during the encoding process.\n";
						QMessageBox::critical(this, tr("HiddenDragon Error"), errorMsg);
						setCursor(Qt::ArrowCursor);
            			return;
                    }
                    else emit encodedImage(imagePtr->outputFileName());
                }
                else if(m_imageFormat==QString("JPEG"))
                {
                    if(imgFormat!=".jpg")
                    {
                        errorMsg+="Please choose the right Image Format!\n";
                    }
                    else if(!imagePtr->EncoderHandler("jpeg", output))
                    {
                        errorMsg += "- An error occured during the encoding process.\n";
						QMessageBox::critical(this, tr("HiddenDragon Error"), errorMsg);
						setCursor(Qt::ArrowCursor);
            			return;
                    }
                    else emit encodedImage(imagePtr->outputFileName());
                }
            }
            if(errorMsg.length()>0)
            {
                QMessageBox::critical(this, tr("HiddenDragon Error"), errorMsg);
            }
            setCursor(Qt::ArrowCursor);
            this->accept();
            setCursor(Qt::ArrowCursor);
            return;

        }
}


