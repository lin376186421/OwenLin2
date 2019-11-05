//
//  CStreamUtils.h
//  CoC
//
//  Created by qsun on 13-6-7.
//
//

#ifndef __CoC__CStreamUtils__
#define __CoC__CStreamUtils__

#include <istream>
#include <stdlib.h>

using namespace std;

typedef long long int64_t;

class CFileStream {
protected:
    const char* pData;
    int mOffset;
public:
    
    CFileStream(const char* _pData)
    {
        pData = _pData;
        mOffset = 0;
    }

    char readByte()
    {
        char value = pData[mOffset++];
        return value;
    }
    
    short readShort()
    {
        char v1 = pData[mOffset++];
        char v2 = pData[mOffset++];
         
        return ((v1 & 0xFF) << 8) | (v2 & 0xFF);
    }
    
    int readInt()
    {
        char v1 = pData[mOffset++];
        char v2 = pData[mOffset++];
        char v3 = pData[mOffset++];
        char v4 = pData[mOffset++];
                
        return (((v1 & 0xFF) << 24) | ((v2 & 0xFF) << 16) | ((v3 & 0xFF) << 8) | (v4 & 0xFF));
    }
    
    float readFloat()
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
    
    bool readBool()
    {
    	bool value = (pData[mOffset++] == 1);
        return value;
    }
    
    const string readUTF()
    {
        int len = this->readShort();
        
        string str(&pData[mOffset], len);
        mOffset += len;
        return str;
    }
    
    void readBytes(char bytes[], int len)
    {
        memcpy(&bytes[0], &pData[mOffset], sizeof(char) * len);
        mOffset += len;
    }
    
    void skipBytes(int len)
    {
        mOffset += len;
    }
    
    int getOffsetIndex() {
        return mOffset;
    }
};

class CStreamReader {
    
protected:
    istream& is;
public:
    
    CStreamReader(istream& is_)
    :is(is_)
    {
    }
      
    char readByte()
    {
        char value;
        
        is.read((char *)(&value),sizeof(char));
        return value;
    }
    
    unsigned char readUnsignedByte()
    {
        unsigned char value = this->readByte();
        return value;
    }
    
    short readShort()
    {
        //short value;
        //is.read((char *)(&value),sizeof(short));
        
        char value0 = this->readByte();
        char value1 = this->readByte();
        
        short value =  ((value0 & 0xff) << 8) | (value1 & 0xff);
        return value;
    }
    
    unsigned short readUnsignedShort()
    {
        unsigned short value = this->readShort();
        return value;
    }
    
    float readFloat()
    {
        int bits = this->readInt();
        
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
    
    int readInt()
    {
        //int value;
        //is.read((char *)(&value),sizeof(int));
        
        short value0 = this->readShort();
        short value1 = this->readShort();
        
        int value = ((value0 & 0xffff) << 16) | (value1 & 0xffff);
        return value;
    }
    
    unsigned int readUnsignedInt()
    {
        unsigned int value = this->readInt();
        return value;
    }
    
    int64_t readLong()
    {
        //long value;
        //is.read((char *)(&value),sizeof(long));
        
        //long 64 bit
        int64_t v1 = this->readInt();
        int64_t v2 = this->readInt();
        
        int64_t value = ((v1 & 0xffffffff) << 32) | (v2 & 0xffffffff);
        
        return value;
    }
    
    unsigned long readUnsignedLong()
    {
        unsigned long value = this->readLong();
        return value;
    }
    
    int read(char values[], int offset, int len)
    {
        if(values == NULL){
            return -1;
        }
        
        is.read(&values[offset], sizeof(char)*len);
        return len;
    }
    
    const string readUTF()
    {
        int len = this->readShort();
        
        char* pBytes = new char[len];
        this->readBytes(pBytes, len);
        
        string value(&pBytes[0], len);
		delete pBytes;
        return value;
    }
    
    void readBytes(char bytes[], int len)
    {
        is.read(&bytes[0], sizeof(char)*len);
    }
    
    int getOffsetIndex()
    {
        return is.cur;
    }
};

class CStreamWriter {
    ostream& os;
public:
    
    CStreamWriter(ostream& os_)
    :os(os_)
    {
    }
    
    void writeByte(char value)
    {
        os.write(&value, sizeof(char));
    }
    
    void writeShort(short value)
    {
        
        char b1 = (value >> 16);
        char b2 = (value & 0xffff);
        
        os.write(&b1, sizeof(char));
        os.write(&b2, sizeof(char));
    }
    
    void writeInt(int value)
    {
        char b1 = (value >> 24) & 0xffff;
        char b2 = (value >> 16) & 0xffff;
        char b3 = (value >> 8) & 0xffff;
        char b4 = (value & 0xffff);
        
        os.write(&b1, sizeof(char));
        os.write(&b2, sizeof(char));
        os.write(&b3, sizeof(char));
        os.write(&b4, sizeof(char));
    }
    
    void writeLong(int64_t value)
    {
        
        int64_t v1 = (value >> 32) & 0xffffffff;
        int v2 = value & 0xffffffff;
        
        this->writeInt(v1);
        this->writeInt(v2);
    }
    
    void writeBytes(char values[], int off, int len)
    {
        int index = 0;
        for (index = 0; index < len; index ++) {
            os.write(&values[off + index], sizeof(char));
        }
    }
    
};

#endif /* defined(__CoC__CStreamUtils__) */
