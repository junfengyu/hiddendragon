#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>
#include <QPointer>
#include <QString>
#include <QDir>
#include "encodeddata.h"


    //! Generic representation of image which can load and hide specific data according to it's format
class Image : public QPixmap, public QObject
    {
    protected:
		QString m_imageFormat;
        //! Embedded data
        QPointer<EncodedData> m_data;
        //! Image file path
        QString m_filePath;
		QString m_outputFilePath;
        //! Image name
        QString m_shortName;
		//! Image width
		int m_width;
        //! Image height
        int m_height;
        //! Indicate when image contains some data
        bool m_isLoaded;

    public:
        Image();
        ~Image();
        Image(const QString& filepath);
        Image(const QPixmap& pixmap, QString filePath="Pixmap");
        Image(const Image& img);
        Image(Image* img);

        QPointer<EncodedData> data() const;
        void setData(QPointer<EncodedData> data);

        QString filePath() const;
        QString baseName() const;
        QString shortName() const;
		QString outputFileName() const;
        void setShortName(const QString shortName);

        bool isLoaded() const;

        int imgWidth() const;
        int imgHeight() const;
        bool EncoderHandler(QString imageFormat, QString outputPath);
		bool DecoderHandler(QString imageFormat, QString outputPath);
        virtual quint32 capacity() const;

        virtual bool load();
        virtual bool compile();
        virtual bool save(QString outputDirPath);

    protected:
        QString fileName(QString filePath);
        void computeNewFileName(QString extension);
    };



#endif
