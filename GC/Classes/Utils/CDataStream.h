//
//  CDataStream.h
//  GameClient
//
//  Created by qsun on 14-3-20.
//
//

#ifndef __GameClient__CDataStream__
#define __GameClient__CDataStream__

#include <string>

#ifndef byte
#define byte char
#endif

using namespace std;

class CDataStream {
    
public:
 
    CDataStream(char* pBytes);
    
    int readInt();
 
    byte readByte();
  
    short readShort();
    
    bool readBool();
  
    float readFloat();
  
    void readBytes(byte* pDest, int len);
    
    string readUTF8();
    
    string readCachedString();
    
public:
    
    char *mBytes;
    int mCurrentByte;
    int mCurrentBit;
};

#endif /* defined(__GameClient__CDataStream__) */
