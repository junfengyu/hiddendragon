#include "image.h"
#include "bmpFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "mman.h"
#include "config.h"
	
#include "arc.h"
	
#include "golay.h"
#include "pnm.h"
#include "jpg.h"
#include "iterator.h"


#ifndef MAP_FAILED
	/* Some Linux systems are missing this */
#define MAP_FAILED	(void *)-1
#endif /* MAP_FAILED */
	
#include "outguess.h"
	
	
	extern int steg_foil;
	extern int steg_foilfail;
	extern int steg_stat;
	extern int steg_offset[MAX_SEEK];
	
	handler *handlers[] = {
		&pnm_handler,
		&jpg_handler
	};
	
	handler *
	get_handler(char *name)
	{
		int i;
	
		if (!(name = strrchr(name, '.')))
			return NULL;
		name++;
	
		for (i = sizeof(handlers)/sizeof(handler *) - 1; i >= 0; i--)
			if (!strcasecmp(name, handlers[i]->extension))
				return handlers[i];
	
		return NULL;
	}


    Image::Image()
        : QPixmap()
    {
        m_shortName = "unamed";
        m_filePath = "/home/";
        m_isLoaded = false;
        setObjectName("Image");
    }

    Image::Image(const QString& filePath)
        : QPixmap(filePath)
    {
        m_filePath = filePath;
        m_shortName = fileName(filePath);
        m_isLoaded = false;
        setObjectName("Image");
        m_width = width();
        m_height = height();
    }

    Image::Image(const QPixmap& pixmap, QString filePath)
        : QPixmap(pixmap)
    {
        m_filePath = filePath;
        m_shortName = fileName(filePath);
        m_isLoaded = false;
        setObjectName("Image");
        m_width = width();
        m_height = height();
    }

    Image::Image(const Image& img)
        : QPixmap(img)
    {
        m_data = img.data();
        m_shortName = img.shortName();
        m_filePath = img.filePath();
        m_width = img.imgWidth();
        m_height = img.imgHeight();
        m_isLoaded = false;
        setObjectName("Image");
    }

    Image::Image(Image* img)
    {
        m_shortName = img->shortName();
        m_filePath = img->filePath();
        m_width = img->imgWidth();
        m_height = img->imgHeight();
        m_isLoaded = false;
        setObjectName("Image");
    }

    Image::~Image()
    {
        // NOTHING TO DO
    }

    QPointer<EncodedData> Image:: data() const
    {
        return m_data;
    }

    void Image::setData(QPointer<EncodedData> data)
    {
        if(!m_data.isNull())
            delete m_data;
        m_data = data;
    }

    QString Image::shortName() const
    {
        return m_shortName;
    }

    void Image::setShortName(const QString shortName)
    {
        m_shortName = shortName;
    }

    QString Image::filePath() const
    {
        return m_filePath;
    }

	QString Image::outputFileName() const
	{
		return m_outputFilePath;
	}

    bool Image::load()
    {
        return false;
    }

    bool Image::isLoaded() const
    {
        return m_isLoaded;
    }

    int Image::imgWidth() const
    {
        return m_width;
    }

    int Image::imgHeight() const
    {
        return m_height;
    }

    quint32 Image::capacity() const
    {
        return 0;
    }

    bool Image::compile()
    {
        return false;
    }

    bool Image::save(QString outputDirPath)
    {
        Q_UNUSED(outputDirPath);
        return false;
    }

    QString Image::baseName() const
    {
        return QDir::fromNativeSeparators(m_filePath).section("/", 0, -2);
    }

    QString Image::fileName(QString filePath)
    {
        return QDir::fromNativeSeparators(filePath).section("/", -1, -1);
    }

    void Image::computeNewFileName(QString extension)
    {
        m_shortName = m_shortName.section('.', 0, -2)+"."+extension;
        m_filePath = baseName()+"/"+m_shortName;
    }

	bool Image::DecoderHandler(QString imageFormat, QString outputPath)
	{
		
		
         char *charFilePath= new char[m_filePath.toStdString().size()+1];
		 strcpy(charFilePath,m_filePath.toStdString().c_str());
		 
		 bool compress = false;	 	 
		 QString tmpFileName;
		 tmpFileName.append(outputPath);
		 tmpFileName.append("/");
         tmpFileName.append("tmp.dat");
		 
		 m_imageFormat=imageFormat;

		 
		 char *chartmpFileName= new char[tmpFileName.toStdString().size()+1];
		 strcpy(chartmpFileName,tmpFileName.toStdString().c_str());

		 if(m_imageFormat==QString("bmp"))
		 {

			 BmpFile bm(charFilePath);
		
		
		 	if( bm.unhide(charFilePath, chartmpFileName) == 0 )
		 	{
				printf("Hidding done.\n");
           
		 	}
		 	else
		 	{
				printf("Error!");
				delete charFilePath;
				
				delete chartmpFileName;
				return false;
		
		 	}
		 
		 	delete charFilePath;
	
		 	delete chartmpFileName;
         	return true;
		 }
		 else if (m_imageFormat==QString("jpeg"))
		{

			FILE *fin = stdin, *fout = stdout;
    		image *image;
    		handler *srch = NULL, *dsth = NULL;
   			char *param = NULL;
    		unsigned char *encdata;
    		bitmap bitmap;	/* Extracted bits that we may modify */
    		iterator iter;
    		int j, ch, derive = 0;
    		stegres cumres, tmpres;
    		config cfg1, cfg2;
    		u_char *data = NULL, *data2 = NULL;
    		int datalen;
    		char *key = "Default key", *key2 = NULL;
    		struct arc4_stream as, tas;
    		char mark = 0, doretrieve = 1;
    		char doerror = 0, doerror2 = 0;

    		int extractonly = 0, foil = 1;

    		steg_stat = 0;

    		memset(&cfg1, 0, sizeof(cfg1));
    		memset(&cfg2, 0, sizeof(cfg2));
    		doretrieve = 1;

    		char* image_file_path = charFilePath;
    		char* retrieve_file = chartmpFileName;

    		srch = get_handler(image_file_path);
    		if (srch == NULL) {
            	fprintf(stderr, "Unknown data type of %s\n", image_file_path);
            	return false;
    		}

    		fin = fopen(image_file_path, "rb");
    		if (fin == NULL) {
            	fprintf(stderr, "Can't open input file '%s': ",
                image_file_path);
            	perror("fopen");
            	return false;
        	}
    		fout = fopen(retrieve_file, "wb");
    		if (fout == NULL) {
            	fprintf(stderr, "Can't open output file '%s': ",
                retrieve_file);
            	perror("fopen");
            	return false;
    		}

    	/* Initialize Golay-Tables for 12->23 bit error correction */
    		if (doerror || doerror2) {
        	fprintf(stderr, "Initalize encoding/decoding tables\n");
       		 init_golay();
    		}

    		fprintf(stderr, "Reading %s....\n", image_file_path);
    		image = srch->read(fin);
    		if (doretrieve)
        	/* Wen extracting get the bitmap from the source handler */
        		srch->get_bitmap(&bitmap, image, STEG_RETRIEVE);

    		fprintf(stderr, "Extracting usable bits:   %d bits\n", bitmap.bits);

    		if (doerror)
        	cfg1.flags |= STEG_ERROR;

        /* Initialize random data stream */
        	arc4_initkey(&as,  "Encryption", key, strlen(key));
        	tas = as;

        	iterator_init(&iter, &bitmap, key, strlen(key));

        	encdata = steg_retrieve(&datalen, &bitmap, &iter, &as,
                    cfg1.flags);

        	data = decode_data(encdata, &datalen, &tas, cfg1.flags);
        	free(encdata);

        	fwrite(data, datalen, sizeof(u_char), fout);
        	free(data);
			fclose(fout);
			fclose(fin);

    		free(bitmap.bitmap);
    		free(bitmap.locked);

    		free_pnm(image);
			delete charFilePath;
	
		 	delete chartmpFileName;

    		return true;


		}
		 

		 
		
    }


	
bool Image::EncoderHandler( QString imageFormat,  QString outputPath)
{
		char *charFilePath= new char[m_filePath.toStdString().size()+1];
		strcpy(charFilePath,m_filePath.toStdString().c_str());
      		
        QString OutPutFileName;
        OutPutFileName.append(outputPath);
        OutPutFileName.append("/");
        OutPutFileName.append("Encoded-");
        OutPutFileName.append(m_shortName);
		
        QString tmpFileName;
        tmpFileName.append(outputPath);
        tmpFileName.append("/");
        tmpFileName.append("hidetmp.dat");

		char *charOutPutFile = new char[OutPutFileName.toStdString().size()+1];
		strcpy(charOutPutFile,OutPutFileName.toStdString().c_str());
		char *chartmpFileName= new char[tmpFileName.toStdString().size()+1];
		strcpy(chartmpFileName,tmpFileName.toStdString().c_str());

        QByteArray hiddenData=m_data->toData()->toByteArray();

		m_imageFormat=imageFormat;
		
		QFile outFile(tmpFileName);
		if (!outFile.open(QIODevice::WriteOnly))
		{
            // qDebug() << "Error opening a file\n";
			 delete charFilePath;
			 delete charOutPutFile;
			 delete chartmpFileName;
			 return false;
		}
		else
		{
  			outFile.write(hiddenData);
  			outFile.close();
		}
		if(m_imageFormat==QString("bmp"))
		{
				BmpFile bm(charFilePath);
				if( bm.hide(charFilePath, chartmpFileName, charOutPutFile) == 0 )
		        {
			        printf("Hidding done.\n");
		        }
		        else
		        {
				    printf("Error!");
				    delete charFilePath;
				    delete charOutPutFile;
				    delete chartmpFileName;
				    return false;
		         }
		    m_outputFilePath = OutPutFileName;
			delete charFilePath;
			delete charOutPutFile;
			delete chartmpFileName;
        	return true;
			}
		else if (m_imageFormat==QString("jpeg"))
		{


		
		FILE *fin = stdin, *fout = stdout;
		image *image;
		handler *srch = NULL, *dsth = NULL;
		char *param = NULL;
		unsigned char *encdata;
		bitmap bitmap;	/* Extracted bits that we may modify */
		iterator iter;
		int j, ch, derive = 0;
		stegres cumres, tmpres;
		config cfg1, cfg2;
		u_char *data2 = NULL;
		int datalen;
		char *key = "Default key", *key2 = NULL;
		struct arc4_stream as, tas;
		char mark = 0, doretrieve = 0;
		char doerror = 0, doerror2 = 0;
		
		int extractonly = 0, foil = 1;
		
		steg_stat = 0;
		
		memset(&cfg1, 0, sizeof(cfg1));
		memset(&cfg2, 0, sizeof(cfg2));
		char *input_file_name=charFilePath;
		char *output_file_name=charOutPutFile;
		
		
		u_char *data=chartmpFileName;   //set embedded data path and file name.
		
		
		
		srch = get_handler(input_file_name);
		if (srch == NULL) {
				fprintf(stderr, "Unknown data type of %s\n", input_file_name);
                return false;
		}
		
		dsth = get_handler(output_file_name);
		if (dsth == NULL) {
			 fprintf(stderr, "Unknown data type of %s\n",
						output_file_name);
					return false;
		}
		
			fin = fopen(input_file_name, "rb");
			if (fin == NULL) {
				fprintf(stderr, "Can't open input file '%s': ",
					input_file_name);
				perror("fopen");
				return false;
			}
			fout = fopen(output_file_name, "wb");
			if (fout == NULL) {
				fprintf(stderr, "Can't open output file '%s': ",
					output_file_name);
				perror("fopen");
				return false;
			}
		
		/* Initialize Golay-Tables for 12->23 bit error correction */
		if (doerror || doerror2) {
			fprintf(stderr, "Initalize encoding/decoding tables\n");
			init_golay();
		}
		
		fprintf(stderr, "Reading %s....\n", input_file_name);
		image = srch->read(fin);
		
		
		/* Initialize destination data handler */
		dsth->init(param);
		/* When embedding the destination format determines the bits */
		dsth->get_bitmap(&bitmap, image, 0);
		
		fprintf(stderr, "Extracting usable bits:   %d bits\n", bitmap.bits);
		
		if (doerror)
			cfg1.flags |= STEG_ERROR;
		
		if (!doretrieve) {
			if (mark)
				cfg1.flags |= STEG_MARK;
			if (foil) {
				dsth->preserve(&bitmap, -1);
				if (bitmap.maxcorrect)
					fprintf(stderr,
						"Correctable message size: %d bits, %0.2f%%\n",
						bitmap.maxcorrect,
						(float)100*bitmap.maxcorrect/bitmap.bits);
			}
		
			do_embed(&bitmap, data, key, strlen(key), &cfg1, &cumres);
		
		
			if (foil) {
				int i, count;
				double mean, dev, sq;
				int n;
				u_char cbit;
				u_char *pbits = bitmap.bitmap;
				u_char *data = bitmap.data;
				u_char *plocked = bitmap.locked;
		
				memset(steg_offset, 0, sizeof(steg_offset));
				steg_foil = steg_foilfail = 0;
		
				for (i = 0; i < bitmap.bits; i++) {
					if (!TEST_BIT(plocked, i))
						continue;
		
					cbit = TEST_BIT(pbits, i) ? 1 : 0;
		
					if (cbit == (data[i] & 0x01))
						continue;
		
					n = bitmap.preserve(&bitmap, i);
					if (n > 0) {
						/* Actual modificaton */
						n = abs(n - i);
						if (n > MAX_SEEK)
							n = MAX_SEEK;
		
						steg_offset[n - 1]++;
					}
				}
		
				/* Indicates that we are done with the image */
				bitmap.preserve(&bitmap, bitmap.bits);
		
				/* Calculate statistics */
				count = 0;
				mean = 0;
				for (i = 0; i < MAX_SEEK; i++) {
					count += steg_offset[i];
					mean += steg_offset[i] * (i + 1);
				}
				mean /= count;
		
				dev = 0;
				for (i = 0; i < MAX_SEEK; i++) {
					sq = (i + 1 - mean) * (i + 1 - mean);
					dev += steg_offset[i] * sq;
				}
		
				fprintf(stderr, "Foiling statistics: "
					"corrections: %d, failed: %d, "
					"offset: %f +- %f\n",
					steg_foil, steg_foilfail,
					mean, sqrt(dev / (count - 1)));
			}
		
			fprintf(stderr, "Total bits changed: %d (change %d + bias %d)\n",
				cumres.changed + cumres.bias,
				cumres.changed, cumres.bias);
			fprintf(stderr, "Storing bitmap into data...\n");
			dsth->put_bitmap (image, &bitmap, cfg1.flags);
		
		
		
			fprintf(stderr, "Writing %s....\n", output_file_name);
			dsth->write(fout, image);
		}
		free(bitmap.bitmap);
		free(bitmap.locked);
		
		free_pnm(image);

		m_outputFilePath = OutPutFileName;
		delete charFilePath;
		delete charOutPutFile;
		delete chartmpFileName;
        return true;
		
				
	}
}

