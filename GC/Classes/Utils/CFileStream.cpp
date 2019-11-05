//
//  CFileStream.cpp
//  GameClient
//
//  Created by qsun on 14-3-20.
//
//

#include "CFileStream.h"
#include  <stdlib.h>

//istream read
CFileInputStream::CFileInputStream(istream& fis)
:fis(fis)
{
    
}

byte CFileInputStream::readByte()
{
    byte value;
    
    fis.read((char *)(&value),sizeof(byte));
	return value;
}

unsigned byte CFileInputStream::readUnsignedByte()
{
    unsigned byte value = readByte();
	return value;
}


unsigned short CFileInputStream::readUnsignedShort()
{
	unsigned short value = readShort();
	return value;
}

short CFileInputStream::readShort()
{
	byte value0 = readByte();
    byte value1 = readByte();
    
	short value =  ((value0 & 0xff) << 8) | (value1 & 0xff);
	return value;
}

long long CFileInputStream::readLong()
{
    
    //long 64 bit
    long long v1 = readInt();
    long long v2 = readInt();
    
    long long value = ((v1 & 0xffffffff) << 32) | (v2 & 0xffffffff);
    
	return value;
}

int CFileInputStream::readInt()
{
    
    short value0 = readShort();
    short value1 = readShort();
    
	int value = ((value0 & 0xffff) << 16) | (value1 & 0xffff);
	return value;
}

float CFileInputStream::readFloat()
{
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

unsigned int CFileInputStream::readUnsignedInt()
{
    unsigned int value = readInt();
	return value;
}

unsigned long CFileInputStream::readUnsignedLong()
{
    unsigned long value = readLong();
	return value;
}

void CFileInputStream::readBytes(byte* dstBytes, int len)
{
    for(int i = 0 ; i < len; i++)
    {
        dstBytes[i] = readByte();
    }
}

const char* CFileInputStream::readUTF()
{
    int len = readShort();
    
    byte* bytes = new byte[len];
    readBytes(bytes, len);
    
    string str(bytes, len);

	delete []bytes;
    
    return str.c_str();
}

string CFileInputStream::readCachedString()
{    
    string str = readUTF();
    return str;
}

//ostream write
CFileOutputStream::CFileOutputStream(ostream& fos)
:fos(fos)
{
    
}

void CFileOutputStream::writeByte(byte value){
    fos.write(&value, sizeof(byte));
}

void CFileOutputStream::writeShort(short value){
    byte b1 = (value >> 16);
    byte b2 = (value & 0xffff);
    
    fos.write(&b1, sizeof(byte));
    fos.write(&b2, sizeof(byte));
}

void CFileOutputStream::writeInt(int value){
    byte b1 = (value >> 24) & 0xffff;
    byte b2 = (value >> 16) & 0xffff;
    byte b3 = (value >> 8) & 0xffff;
    byte b4 = (value & 0xffff);
    
    fos.write(&b1, sizeof(byte));
    fos.write(&b2, sizeof(byte));
    fos.write(&b3, sizeof(byte));
    fos.write(&b4, sizeof(byte));
}

void CFileOutputStream::writeFloat(float value)
{
    fos.write((const char*)&value, sizeof(float));
}

void CFileOutputStream::writeLong(long long value)
{
    long long v1 = (value >> 32) & 0xffffffff;
    int v2 = value & 0xffffffff;
    
    writeInt(v1);
    writeInt(v2);
}

void CFileOutputStream::writeBytes(byte* values, int off, int len){
    for (int index = 0; index < len; index ++) {
        fos.write(&values[off + index], sizeof(byte));
    }
}
