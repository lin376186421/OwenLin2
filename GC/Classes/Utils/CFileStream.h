//
//  CFileStream.h
//  GameClient
//
//  Created by qsun on 14-3-20.
//
//

#ifndef __GameClient__CFileStream__
#define __GameClient__CFileStream__

#include <fstream>

#ifndef byte
#define byte char
#endif

using namespace std;

class CFileInputStream {
public:
    
    CFileInputStream(istream& fis);
    
    byte readByte();
    unsigned byte readUnsignedByte();
    
    short readShort();
    unsigned short readUnsignedShort();

    int readInt();
    unsigned int readUnsignedInt();
    
    float readFloat();
    
    long long readLong();
    unsigned long readUnsignedLong();
    
    void readBytes(byte* dstBytes, int len);
    
    const char* readUTF();
    
    string readCachedString();
public:
    istream& fis;
};

class CFileOutputStream {
    
public:
    
    CFileOutputStream(ostream& fos);
    
    void writeByte(byte value);
    void writeShort(short value);
    void writeInt(int value);
    void writeFloat(float value);
    void writeLong(long long value);
    void writeBytes(byte* values, int off, int len);
    
public:
    ostream& fos;
};

#endif /* defined(__GameClient__CFileStream__) */
