#include "blackdragon.h"

char* BlackDragon::longToByteArray(ulonglong value){
   char *b;
  b=( char*)malloc(sizeof(char)*8);
  int i;
  for(i = 0; i < 8;i++){
    int offset = (8 - 1 - i) * 8;
    b[i] = (char) ((value  >> offset) & 0xFFFFFFFL);


  }
  return b;
}


void BlackDragon::XOR(QByteArray source, QByteArray &target, QByteArray &key, int length)
{

    int count=0;
    for(count;count<length;count++)
    {
        target[count]=source[count]^key[count];
     }
    return;
}


QPointer<EncodedData> BlackDragon::encode(QString key, QPointer<EncodedData> msg)
{

	int msgLength = msg->bytes().length();
	int dataLength=0;
    if(msgLength%8)
		dataLength = msgLength/8+1;
	else dataLength= msgLength/8;
	int kslength= dataLength/2;
    unsigned char* ksByteArray;
	  //allocate and initiate
    ulong iv[] = { 0, 0, 0, 0 };
	QByteArray encodedMsg= QByteArray(msgLength,0);
    QByteArray ksArray=QByteArray();
   // QByteArray &originalMsg;
		
	ulong masterkey_ulong[]={0,0,0,0};
	ulonglong* ks = (ulonglong*)malloc((dataLength)*sizeof(ulonglong));
			    
    //generate keystream

    Dragon2_Ctx ctx;
    BD_initkey(&ctx, masterkey_ulong);
    BD_initiv(&ctx, iv);
    BD_keystream(&ctx, ks, kslength);
    ksByteArray=(unsigned char*)ks;
    ksArray.insert(0,ksByteArray,msgLength);
    /*for(i=0;i<dataLength;i++){
        ksArray.append((char *)ksByteArray); //(longToByteArray(ks[i])

    }*/
    //originalMsg=msg->bytes();
    XOR(msg->bytes(), encodedMsg, ksArray, msgLength);
	delete msg;

    return new EncodedData(encodedMsg, Data::BYTES, false);

}


QByteArray BlackDragon::decode(QString key, QByteArray &encodedData)
{

    int msgLength = encodedData.length();
    int dataLength=0;
    if(msgLength%8)
        dataLength = msgLength/8+1;
    else dataLength= msgLength/8;
    int kslength= dataLength/2;

      //allocate and initiate
    ulong iv[] = { 0, 0, 0, 0 };
    QByteArray decodedMsg= QByteArray(msgLength,0);
    QByteArray ksArray=QByteArray();
   // QByteArray &originalMsg;
    unsigned char* ksByteArray;
    ulong masterkey_ulong[]={0,0,0,0};
    ulonglong* ks = (ulonglong*)malloc((dataLength)*sizeof(ulonglong));

    //generate keystream

    Dragon2_Ctx ctx;
    BD_initkey(&ctx, masterkey_ulong);
    BD_initiv(&ctx, iv);
    BD_keystream(&ctx, ks, kslength);
    ksByteArray=(unsigned char*)ks;
    ksArray.insert(0,ksByteArray,msgLength);
    /*int i=0;
    for(i=0;i<dataLength;i++){
      ksArray.append((char*)ksByteArray);
    }*/
    XOR(encodedData, decodedMsg, ksArray, msgLength);
    return decodedMsg;

}

