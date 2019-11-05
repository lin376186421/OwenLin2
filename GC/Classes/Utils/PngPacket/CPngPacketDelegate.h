//
//  CPngPacketDelegate.h
//  PngPacket
//
//  Created by qsun on 13-6-14.
//
//

#ifndef __Utils__CPngPacketDelegate__
#define __Utils__CPngPacketDelegate__

class CPngPacketDelegate {
public:
    virtual void onDecodeSuccessed(std::string fileName, int pixelFormat, int pixelBit, int width, int height, unsigned char pixels[]) = 0;
};

#endif /* defined(__Utils__CPngDecodeDelegate__) */
