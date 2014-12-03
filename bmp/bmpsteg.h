#ifndef BMPSTEG_H
#define BMPSTEG_H

class BmpSteg
{
public:

        BmpSteg(char *filename);                                		//constructor #
        ~BmpSteg();                                             		//destructor
        int isFileExist(char *filename);								//checking file existence

        int hide(char *bmpfile, char *msgfile, char *output);			//hidding job *
        int unhide(char *bmpfile,char *msgfile);


        long getBmpFileCapacity(char *bmpfile);
private:
        // properties...
        int  bmpIdentifier;												//BM identifier
        long bmpFilesize;												//File size
        long m_capacity;
        unsigned short int bmpres1,bmpres2;
        long bmpPixoff;
        long bmpiSize;
        long bmpWidth;													//BMP width
        long bmpHeight;													//BMP height
        unsigned short int bmpPlanes;
        unsigned short int bmpBitsPixel;
        long bmpCompression;											//if BMP is compressed
        long bmpImageSize;
        long bmpXscale;
        long bmpYScale;
        long bmpColor;													//BMP color depth
        long bmpImpCol;
        long bmpTotalStuffablechar;
        int readHeader(char *filename);



};

#endif // BMPSTEG_H
