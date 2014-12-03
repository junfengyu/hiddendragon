#include "splitfile.h"
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "dbg.h"

const int BUFFER_SIZE= 8096;
const int NAME_SIZE=2048;
SplitFile::SplitFile(QString fileName, int pieces, QObject *parent) :
    QObject(parent)
{
    m_fileName=QString::null;
    m_pieces=0;
    m_outputFileName=QString::null;
    if(isFileExist(fileName))
    {
        m_fileName=fileName;
        m_pieces=pieces;
        m_outputFileName=fileName.append(".out");
    }

}

bool SplitFile::isFileExist(QString filename)
{

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return false;
        file.close();
        return true;
}




SplitFile::~SplitFile()
{


}

int SplitFile::doSplit()
{
    if(m_fileName==QString::null)
        return -1;
    int peices=m_pieces;
    char *file_path= new char[m_fileName.toStdString().size()+1];
    strcpy(file_path,m_fileName.toStdString().c_str());

    void *buffer = malloc(BUFFER_SIZE);
    char *p_name = malloc(NAME_SIZE);
    FILE *fh = fopen(file_path, "rb");
    struct stat stbuf;
    check(stat(file_path, &stbuf) != -1, "Failed to get stat");
    const double p_size = ceil(stbuf.st_size / peices);
    int i = 0;
    long cur_pos = 0;
    long bf_size;
    long max_ftell;
        for (; i < peices; i++) {
            sprintf(p_name, "%s.%d", file_path, i);
            bf_size = BUFFER_SIZE;
            if (i == peices - 1)
                max_ftell = stbuf.st_size;
            else
                max_ftell = p_size * (i + 1);

            cur_pos = ftell(fh);

            FILE *fp = fopen(p_name, "wb");
            check(fp, "Failed to open out file stream to : %s", p_name);

            while (ftell(fh) <= max_ftell && fread(buffer, bf_size, 1, fh)) {
                fwrite(buffer, bf_size, 1, fp);
                if ((max_ftell - ftell(fh)) < BUFFER_SIZE)
                    bf_size = max_ftell - ftell(fh);



            }

            fclose(fp);

        }

        free(buffer);
        free(p_name);
        fclose(fh);
        return 1;  //success

        error:
            free(buffer);
            free(p_name);
            return 0; //0 error occured
}

int doJoin()
{

    long buf_size;
    char *p_name = malloc(NAME_SIZE);
    char *mf_name = malloc(NAME_SIZE);
    void *buffer = malloc(BUFFER_SIZE);

    file_path
    else
       strcpy(mf_name, out_filename);

        log_info("Joining files to: %s", mf_name);
        FILE *fh = fopen(mf_name, "wb");

        struct stat stbuf;

        int i = 0;
        sprintf(p_name, "%s.%d", file_path, i);
        while (stat(p_name, &stbuf) != -1) {

            buf_size = BUFFER_SIZE;
            log_info("Peice: %s", p_name);

            //don't need to check here checked in the main while loop above ^
            FILE *fp = fopen(p_name, "rb");

            while (fread(buffer, buf_size, 1, fp)) {
                fwrite(buffer, buf_size, 1, fh);

                if ((stbuf.st_size - ftell(fp)) < BUFFER_SIZE) {
                    buf_size = stbuf.st_size - ftell(fp);
    //				printf("BUFFER SIZE: %d\n", buf_size);
                }

            }

            i++;
            sprintf(p_name, "%s.%d", file_path, i);
            fclose(fp);

        }

        if(i>0)
            log_info("done joining %d files to %s", i, mf_name);
        else
            log_info("Failed to find peices");

        free(p_name);
        free(mf_name);
        free(buffer);
        fclose(fh);
        return 1;

        error:
            free(p_name);
            free(mf_name);
            free(buffer);
            return 0;


}
