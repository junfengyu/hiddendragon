#include "decodedialog.h"
#include "controller.h"
#include "blackdragon/blackdragon.h"
#include "splitfile.h"


DecodeDialog::DecodeDialog(QWidget* parent)
        : OptionDialog(parent)
{
        this->setObjectName("DecodeDialog");
        m_logger->setClassName("DecodeDialog");
        m_checkSize = false;

        this->setWindowTitle("Decode message");
        this->setWindowIcon(QPixmap(QString::fromUtf8(":/icons/decode.png")));

        okButton->setText("Decode");
        iconLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/decode.png")));
        msgLabel->setText("Decoded message");
        fileNameLabel->setText("");
        fileButton->setEnabled(false);
        fileRemoveButton->setVisible(false);
        
       	defaultCheckBox->setVisible(false);
		destinationLabel->setVisible(false);
		destinationLineEdit->setVisible(false);
		toolButton->setVisible(false);
        msgTextEdit->setReadOnly(true);
        dataWidget->setVisible(false);
        orLabel->setVisible(false);
       	capacityLabel->setVisible(false);
        dropboxCheckBox->setVisible(false);
        connect(fileButton, SIGNAL(clicked(bool)), this, SLOT(selectFile()));
        //connect(fileButton, SIGNAL(released()), this, SLOT(releaseFileButton()));
}

DecodeDialog::~DecodeDialog()
{
}

void DecodeDialog::showEvent(QShowEvent* event)
{
        if(!event->spontaneous())
        {
            msgTextEdit->setText("");
            fileButton->setEnabled(false);
            cryptoPwd1LineEdit->setText("");
            cryptoPwd2LineEdit->setText("");
            checkCryptoPassword();
            imageFormatChanged(imageFormatComboBox->currentText());
            removeFile();
            if(!m_img.isNull())
                this->setWindowTitle("Decode message: " + m_img->filePath());
            dataWidget->setVisible(false);
          
        }
}

   
void DecodeDialog::ok()
{
        if(!isDialogReady())
            return;
        msgTextEdit->setText("");
        removeFile();
        QDir dir;  
		QString pwd = dir.absolutePath();
        QList <QByteArray> bufferList;
        QString  errorMsg;
        if(!multiImageBox->isChecked())
        {
            QString  imgName=m_img->shortName();
            int index=imgName.lastIndexOf(".");
            QString imgFormat=imgName.right(imgName.length()-index);

            if(m_imageFormat==QString("BMP"))
            {
                if(imgFormat!=".bmp")
                {
                    QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "Please choose the correct image format.");
                    setCursor(Qt::ArrowCursor);
                    return;
                }
				if(!m_img->DecoderHandler("bmp", pwd))	
				{
		   			QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "An error occured during the encoding process.");
                    setCursor(Qt::ArrowCursor);
                    return;
				}
            }else if(m_imageFormat==QString("JPEG"))
            {
                if(imgFormat!=".jpg")
                {
                    QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "Please choose the correct image format.");
                    setCursor(Qt::ArrowCursor);
                    return;
                }
				if(!m_img->DecoderHandler("jpeg", pwd))	
				{
		   			QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "An error occured during the encoding process.");
                    setCursor(Qt::ArrowCursor);
                    return;
				}
            }
        
            QString tmpFileName;
            tmpFileName.append(pwd);
            tmpFileName.append("/");
            tmpFileName.append("tmp.dat");
            QFile file(tmpFileName);
            if (!file.open(QIODevice::ReadOnly))
                     return;
            QByteArray decodingBuffer = file.readAll();
            file.close();

            QByteArray decryptedData = BlackDragon::decode(cryptoPwd2LineEdit->text(), decodingBuffer.remove(0, 1));

        
            char format=decryptedData.at(0);
            if(format=='5'){
                    msgLabel->setText("Decoded file");
                    decryptedData.remove(0, 1);
                    int indexFileName=decryptedData.indexOf("<",1);
                    QByteArray getFileName=decryptedData.left(indexFileName);
                    QString decodedMsgFileName;
                    decodedMsgFileName.append(pwd);
                    decodedMsgFileName.append("/");
                    decodedMsgFileName.append(QTextCodec::codecForMib(106)->toUnicode(getFileName));
                    decryptedData.remove(0,indexFileName+1);
                    QFile file(decodedMsgFileName);
                    file.open(QIODevice::WriteOnly);
                    file.write(decryptedData);
                    file.close();

                    m_filePath = decodedMsgFileName;
					m_fileShortName=QTextCodec::codecForMib(106)->toUnicode(getFileName);
                    fileNameLabel->setText(QTextCodec::codecForMib(106)->toUnicode(getFileName)); //m_filePath;
                    msgTextEdit->setVisible(false);
                    fileWidget->setVisible(true);
                    fileButton->setEnabled(true);
                    dataWidget->setVisible(true);
            }
            else if(format=='2'){
                    msgLabel->setText("Decoded message");
                    decryptedData.remove(0, 1);
                    //QString dataAsString = QTextCodec::codecForMib(106)->toUnicode(decryptedData);


                    char* decryptedStr = decryptedData.data();


                    QString dataAsString=QString::fromAscii(decryptedStr);

                    msgTextEdit->setText(dataAsString);
                    msgTextEdit->setVisible(true);
                    fileWidget->setVisible(false);
                    dataWidget->setVisible(true);
            }else{
                    QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     "This image don't seem to have an hidden message.");
            }
        }else
        {
            QString  imgName, imgFormat;

           foreach(QPointer<Image> imagePtr, m_outputImageFullNameMap)
            {

                imgName=m_img->shortName();
                int index=imgName.lastIndexOf(".");
                imgFormat=imgName.right(imgName.length()-index);

                if(m_imageFormat==QString("BMP"))
                {
                    if(imgFormat!=".bmp")
                    {
                        QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                             "Please choose the correct image format.");
                        setCursor(Qt::ArrowCursor);
                        return;
                    }

                    if(!imagePtr->DecoderHandler("bmp", pwd))
                    {
                        QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "An error occured during the encoding process.");
                    }
                }else if(m_imageFormat==QString("JPEG"))
                {
                    if(imgFormat!=".jpg")
                    {
                        QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                             "Please choose the correct image format.");
                        setCursor(Qt::ArrowCursor);
                        return;
                    }
                    if(!imagePtr->DecoderHandler("jpeg", pwd))
                    {
                        QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "An error occured during the encoding process.");
                    }

                }
                QString tmpFileName;
                tmpFileName.append(pwd);
                tmpFileName.append("/");
                tmpFileName.append("tmp.dat");
                QFile file(tmpFileName);
                if (!file.open(QIODevice::ReadOnly))
                         return;
                QByteArray decodingBuffer = file.readAll();
                file.close();
                bufferList << decodingBuffer;

            }
            QString decodedMsgFileName;
            QList<QString> msgFileList;
            QByteArray getFileName;
            foreach(QByteArray eachBuffer,bufferList)
            {
                QByteArray decryptedData = BlackDragon::decode(cryptoPwd2LineEdit->text(), eachBuffer.remove(0, 1));
                char format=decryptedData.at(0);
                if(format=='5'){
                    msgLabel->setText("Decoded file");
                    decryptedData.remove(0, 1);
                    int indexFileName=decryptedData.indexOf("<",1);
                    getFileName=decryptedData.left(indexFileName);

                    decodedMsgFileName.append(pwd);
                    decodedMsgFileName.append("/");
                    decodedMsgFileName.append(QTextCodec::codecForMib(106)->toUnicode(getFileName));
                    decryptedData.remove(0,indexFileName+1);
                    QFile file(decodedMsgFileName);
                    file.open(QIODevice::WriteOnly);
                    file.write(decryptedData);
                    file.close();
                    msgFileList<< decodedMsgFileName;
                    decodedMsgFileName.clear();

                }else{
                    QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     "This image don't seem to have an hidden message.");
                }
            }
                SplitFile dataFile(msgFileList);
                dataFile.doJoin();
                QString &decodedJointedMsgFileName=dataFile.getOriginalFileName();
                int index=decodedJointedMsgFileName.lastIndexOf("/");
                QString originalFileName=decodedJointedMsgFileName.right(index);
                m_filePath = decodedJointedMsgFileName;
                m_fileShortName=originalFileName;//QTextCodec::codecForMib(106)->toUnicode(originalFileName);
                fileNameLabel->setText(originalFileName); //m_filePath;
                msgTextEdit->setVisible(false);
                fileWidget->setVisible(true);
                fileButton->setEnabled(true);
                dataWidget->setVisible(true);

        }

}

void DecodeDialog::releaseFileButton()
{
        if (!fileButton->isDown())
        {
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;

            mimeData->setData("application/octet-stream", m_filePath.toUtf8());
            drag->setMimeData(mimeData);

            drag->exec(Qt::MoveAction);
        }
}


