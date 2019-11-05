//
//  CDataStream.cpp
//  GameClient
//
//  Created by qsun on 14-3-20.
//
//

#include "CDataStream.h"
#include  <stdlib.h>

CDataStream::CDataStream(char* pBytes)
{
    mBytes = pBytes;
    mCurrentByte = 0;
    mCurrentBit = 0;
}

byte CDataStream::readByte() {
    byte b = this->mBytes[this->mCurrentByte];
    this->mCurrentByte++;
    return b;
}

short CDataStream::readShort() {
    return (short) ( ((readByte() & 0xFF) << 8) + (readByte() & 0xFF) );
}

bool CDataStream::readBool() {
    return 0 == this->readByte() ? false : true;
}

string CDataStream::readUTF8()
{
    int len = readShort();
    
    byte* bytes = new byte[len];
    readBytes(bytes, len);
    
    string str(bytes, len);
    delete []bytes;
    return str;
}

int CDataStream::readInt() {
    
	int value = ((readShort() & 0xffff) << 16) | (readShort() & 0xffff);
    return value;
}


float CDataStream::readFloat() {
    int bits = readInt();
    
    if(bits == 0)return 0.0f;
    
    int s = ((bits >> 31) == 0) ? 1 : -1;
    int e = ((bits >> 23) & 0xff);
    int m = (e == 0) ?
    (bits & 0x7fffff) << 1 :
    (bits & 0x7fffff) | 0x800000;
    
    double ee = 1 << abs(e - 150);
    if (e - 150 < 0) ee = 1 / ee;
    
    return s * m * ee;
}

std::string CDataStream::readCachedString() {
    string str = readUTF8();
    return str;
}

void CDataStream::readBytes(byte* dstBytes, int len)
{
    for(int i = 0 ; i < len; i++)
    {
        dstBytes[i] = readByte();
    }
}
