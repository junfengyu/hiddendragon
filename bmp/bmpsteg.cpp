#include "bmpsteg.h"
#include <iostream>
#include <cstdlib>
#include <string.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


struct Messageheader{
    unsigned int messagelength;
    char messagefiletype[4];
} messageheader;

// class contructor
BmpSteg::BmpSteg(char *filename)
{
    if(isFileExist(filename) == 0)
    {
        readHeader(filename);
    }
}

BmpSteg::~BmpSteg ()
{}

int BmpSteg::isFileExist(char *filename)
{
        FILE *bmpFilename;
        bmpFilename = fopen(filename,"rb");

        if(bmpFilename == NULL)
        {
            throw("File doesn't exits or wrong Filename.\n");
            return (-1);
        }

        fclose(bmpFilename);
        return 0;
}

// read header of file and other stuff, important method.
int BmpSteg::readHeader(char *filename)
{
        char m1,m2;
        unsigned long width, height, fileSize;

        if(isFileExist(filename) == 0)
        {
            FILE *bmpFilename = fopen(filename,"rb");

            // reading first two part of file.
            fread((char *)&m1,1,1,bmpFilename);
            fread((char *)&m2,1,1,bmpFilename);

            if(m1 != 'B' || m2 != 'M')	//B 42 & M 4D
            {
                throw ("error: not a valid bitmap file");
                fclose(bmpFilename);
                return -1;
            }

            bmpIdentifier = m1 * 100 + m2;

            fread((long *)&bmpFilesize,4,1,bmpFilename);

            fread((unsigned short int *)&bmpres1,2,1,bmpFilename);

            fread((unsigned short int *)&bmpres2,2,1,bmpFilename);

            fread((long *)&bmpPixoff,4,1,bmpFilename);

            fread((long *)&bmpiSize,4,1,bmpFilename);

            fread((long *)&bmpWidth,4,1,bmpFilename);

            fread((long *)&bmpHeight,4,1,bmpFilename);

            fread((unsigned short int *)&bmpPlanes,2,1,bmpFilename);

            fread((unsigned short int *)&(bmpBitsPixel),2,1,bmpFilename);

            fread((long *)&bmpCompression,4,1,bmpFilename);

            fread((long *)&bmpImageSize,4,1,bmpFilename);

            fread((long *)&bmpXscale,4,1,bmpFilename);

            fread((long *)&bmpYScale,4,1,bmpFilename);

            fread((long *)&bmpColor,4,1,bmpFilename);

            fread((long *)&bmpImpCol,4,1,bmpFilename);

            fclose(bmpFilename);

            width = bmpWidth;
            height = bmpHeight;
            fileSize = width * height * 3;
            bmpTotalStuffablechar = (fileSize/8)-54;

            return 0;
        }
        return -1;
}

int BmpSteg::hide(char *bmpfile, char *msgfile, char *output)
{


    int i,j,k,l,t;
    IplImage* coverimg = cvLoadImage(bmpfile);
    IplImage* stegoimg = coverimg;

    CvScalar pix;
    int h=coverimg->height;
    int w=coverimg->width;

    int nchannel = 3;
    int nplane = 3;
    int charsize = 8; //a character has 8 bits


    int messagelength;
    char *message;
    int maxFileSize=(int)(h*w*nchannel*nplane/charsize);


    FILE *f = fopen(msgfile, "rb");
    char* stringbuffer=NULL;
    int fsize=0;
    if(f!=NULL){
        fseek(f, 0, SEEK_END);
        fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        stringbuffer = (char*)malloc(fsize + 1);
        fread(stringbuffer, fsize, 1, f);
        fclose(f);
        remove(msgfile);
        stringbuffer[fsize] = 0;
    }else return -1;

    if(fsize>=maxFileSize)
        return -1;

    message=stringbuffer;
    messagelength=fsize;

    int n=messagelength*charsize; //number of message bits


    n = (n > h*w*nchannel*nplane - sizeof(messageheader)*charsize) ? h*w*nchannel*nplane - sizeof(messageheader)*charsize: n; //number of bits to be embedded, h*w is the array size, 3 channels, and 3 LSB planes
    n = n/charsize * charsize;//round to char

    messageheader.messagelength = n/charsize;
    std::string messagefilename(msgfile);
    std::string messagefiletype = (messagefilename.substr(messagefilename.find_last_of(".") + 1));


    for(t=0;t<sizeof(messageheader.messagelength)*charsize;t++){//embed messagelength
        j=t%w;//which column
        i=(t/w)%h;//which row
        k=(t/(h*w))%nchannel;//which channel
        l=(t/(h*w*nchannel))%nplane; //which LSB plane
        pix=cvGet2D(stegoimg,i,j); // has 3 channels

        pix.val[k] += (signed)((((messageheader.messagelength>>t)&1) << l)-(int(pix.val[k]) & (1u<<l)));

        cvSet2D(stegoimg,i,j,pix);
    }

    for(t=sizeof(messageheader.messagelength)*charsize;t<sizeof(messageheader)*charsize;t++){//embed message filetype
        j=t%w;//which column
        i=(t/w)%h;//which row
        k=(t/(h*w))%nchannel;//which channel
        l=(t/(h*w*nchannel))%nplane; //which LSB plane
        pix=cvGet2D(stegoimg,i,j); // has 3 channels

        int pos = t - sizeof(messageheader.messagelength)*charsize;
        pix.val[k] += (signed)(((((messagefiletype[pos/charsize])>>(pos%charsize))&1) << l)-(int(pix.val[k]) & (1u<<l)));

        cvSet2D(stegoimg,i,j,pix);
    }

    for(t=sizeof(messageheader)*charsize;t<sizeof(messageheader)*charsize+n;t++){//which bit
        j=t%w;//which column
        i=(t/w)%h;//which row
        k=(t/(h*w))%nchannel;//which channel
        l=(t/(h*w*nchannel))%nplane; //which LSB plane
        pix=cvGet2D(stegoimg,i,j); // has 3 channels

        int pos = t - sizeof(messageheader)*charsize;

        pix.val[k] += (signed)((((message[pos/charsize]>>(pos%charsize))&1) << l)-(int(pix.val[k]) & (1u<<l)));

    cvSet2D(stegoimg,i,j,pix);
    }


    cvSaveImage(output,stegoimg);
    free(message);

    return 0;
}

int BmpSteg::unhide (char *bmpfile, char *msgfile)
{

        int i,j,k,l,t;
        IplImage* stegoimg = cvLoadImage(bmpfile);
        CvScalar pix;
        int h=stegoimg->height;
        int w=stegoimg->width;
        int nchannel = 3;
        int nplane = 3;
        int charsize = 8; //a character has 8 bits

        memset(&messageheader, 0, sizeof(messageheader));


        for(t=0;t<sizeof(messageheader.messagelength)*charsize;t++){//extract messagelength
            j=t%w;//which column
            i=(t/w)%h;//which row
            k=(t/(h*w))%nchannel;//which channel
            l=(t/(h*w*nchannel))%nplane; //which LSB plane
            pix=cvGet2D(stegoimg,i,j); // has 3 channels

            messageheader.messagelength += (int(pix.val[k]) & (1u<<l))<<(t);

        }
        for(t=sizeof(messageheader.messagelength)*charsize;t<sizeof(messageheader)*charsize;t++){//embed message filetype
            j=t%w;//which column
            i=(t/w)%h;//which row
            k=(t/(h*w))%nchannel;//which channel
            l=(t/(h*w*nchannel))%nplane; //which LSB plane
            pix=cvGet2D(stegoimg,i,j); // has 3 channels

            int pos = t - sizeof(messageheader.messagelength)*charsize;
            messageheader.messagefiletype[pos/charsize] += (int(pix.val[k]) & (1u<<l))<<(pos%charsize);
        }

        unsigned char *extractedmessage = new unsigned char [messageheader.messagelength+1];
        for(t=0;t<messageheader.messagelength;t++){
            extractedmessage[t] = '\0';
        }


            //data extraction start
        for(t=sizeof(messageheader)*charsize;t<(sizeof(messageheader)+messageheader.messagelength)*charsize;t++){
            j=t%w;//which column
            i=(t/w)%h;//which row
            k=(t/(h*w))%nchannel;//which channel
            l=(t/(h*w*nchannel))%nplane; //which LSB plane
            pix=cvGet2D(stegoimg,i,j); // has 3 channels
            int pos = t - sizeof(messageheader)*charsize;
            extractedmessage[pos/charsize] += (unsigned char)(((int(pix.val[k]) >> l)&1)<<(pos%charsize));
        }


            FILE *fp;
           // char outputMsgFilePath[100]="/sdcard/receiver/ReceiveEncryptedFile.";
           // strcat(outputMsgFilePath,messageheader.messagefiletype);
            fp = fopen(msgfile,"wb+");
            if(fp!=NULL){
              fwrite(extractedmessage, 1 , messageheader.messagelength , fp );
              fclose(fp);
              delete[] extractedmessage;
              return 0;
            }
            else{
              delete[] extractedmessage;
              return -1;
            }
}

long BmpSteg::getBmpFileCapacity(char *bmpfile)
{
    int i,j,k,l,t;
    IplImage* coverimg = cvLoadImage(bmpfile);
    IplImage* stegoimg = coverimg;

    CvScalar pix;
    int h=coverimg->height;
    int w=coverimg->width;

    int nchannel = 3;
    int nplane = 3;
    int charsize = 8; //a character has 8 bits


    int messagelength;
    char *message;
    long maxFileSize=(long)(h*w*nchannel*nplane/charsize);
    return maxFileSize;
}
