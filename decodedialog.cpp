#include "decodedialog.h"
#include "controller.h"


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
/*
        if(!currentFormatModule()->isDecodeWidgetReady())
        {
            QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                 QString("Wrong options for the current encoding image format.\n")+
                                 "Please check your options values before launching the decoding process.");
            return;
        }
        */ //!!!!!!!!!!!!! ethan


       
        QDir dir;  
		QString pwd = dir.absolutePath();

		if(m_imageFormat==QString("BMP"))
		{
				if(!m_img->DecoderHandler("bmp", pwd))	
				{
		   			QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                         "An error occured during the encoding process.");
				}
		}else if(m_imageFormat==QString("JPEG"))
		{
				if(!m_img->DecoderHandler("jpeg", pwd))	
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
        QByteArray decodedBuffer = file.readAll();
        file.close();
        char format=decodedBuffer.at(0);
        if(format=='5'){
                    msgLabel->setText("Decoded file");
                    decodedBuffer.remove(0, 1);
                    int indexFileName=decodedBuffer.indexOf("<",1);
                    QByteArray getFileName=decodedBuffer.left(indexFileName);
                    QString decodedMsgFileName;
                    decodedMsgFileName.append(pwd);
                    decodedMsgFileName.append("/");
                    decodedMsgFileName.append(QTextCodec::codecForMib(106)->toUnicode(getFileName));
                    decodedBuffer.remove(0,indexFileName+1);
                    QFile file(decodedMsgFileName);
                    file.open(QIODevice::WriteOnly);
                    file.write(decodedBuffer);
                    file.close();

                    m_filePath = decodedMsgFileName;
                    fileNameLabel->setText(QTextCodec::codecForMib(106)->toUnicode(getFileName)); //m_filePath;
                    msgTextEdit->setVisible(false);
                    fileWidget->setVisible(true);
                    fileButton->setEnabled(true);
                    dataWidget->setVisible(true);
        }
        else if(format=='2'){
                    msgLabel->setText("Decoded message");
                    decodedBuffer.remove(0, 1);
                    QString dataAsString = QTextCodec::codecForMib(106)->toUnicode(decodedBuffer);
                    msgTextEdit->setText(dataAsString);
                    msgTextEdit->setVisible(true);
                    fileWidget->setVisible(false);
                    dataWidget->setVisible(true);
        }else{
                    QMessageBox::warning(this, tr("HiddenDragon Warning"),
                                     "This image don't seem to have an hidden message.");
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


