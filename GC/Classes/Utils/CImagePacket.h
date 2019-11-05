//
//  CImagePacket.h
//  RobinHood
//
//  Created by qsun on 13-6-14.
//
//

#ifndef __Utils__CImagePacket__
#define __Utils__CImagePacket__

#include "cocos2d.h"
#include "PngPacket/CPngPacketDelegate.h"

using namespace cocos2d;
using namespace std;

#define DECODE_SERCET_KEY "C4AR00F4"

class CImagePacket : public CPngPacketDelegate
{
public:

    static CImagePacket* sharedImagePacket();
    
    void init();
    
    static void setSercetKey();
    
    static void decodePacket(const char* filePath);
    
    void onDecodeSuccessed(string fileName, int pixelFormat, int colorBit, int width, int height, unsigned char* pixels);
    
    bool reloadImage;
};


class CImageCache : public CCTextureCache {
public:
    static void addTexture(string key, CCTexture2D* tex);
};
#endif /* defined(__Utils__CImagePacket__) */
