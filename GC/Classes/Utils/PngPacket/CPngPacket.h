//
//  CPngPacket.h
//  PngPacket
//
//  Created by qsun on 13-6-7.
//
//

#ifndef __Utils__CPngPacket__
#define __Utils__CPngPacket__

#include <fstream>
#include "CStreamUtils.h"
#include "CPngPacketDelegate.h"

#define PNG_DECODE_VERSION 2.0

typedef unsigned char byte;

class CPngPacket{

public:

    static CPngPacket* getInstance();
    
    void decodePacket(unsigned char* pData, unsigned long size);
    void rebuildImage(byte idatData[], int flags, int pixelBit, int width, int height, int dataSize);
    
    void setSercetKey(string sercetKey);
    void setDelegate(CPngPacketDelegate* pDelegate_);
private:
    CPngPacketDelegate* pDelegate;
};

#endif /* defined(__Utils__CPngPacketDecode__) */
