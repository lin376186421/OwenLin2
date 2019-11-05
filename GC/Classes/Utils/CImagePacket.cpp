//
//  CImagePacket.cpp
//  RobinHood
//
//  Created by qsun on 13-6-14.
//
//

#include "CImagePacket.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "Utils/PngPacket/CPngPacket.h"
#else
#include "CPngPacket.h"
#endif

static CImagePacket* s_pImagePacket = NULL;
CImagePacket* CImagePacket::sharedImagePacket()
{
    if (s_pImagePacket == NULL) {
        s_pImagePacket = new CImagePacket();
        s_pImagePacket->init();
    }
    return s_pImagePacket;
}

void CImagePacket::init()
{
    reloadImage = false;
    CPngPacket::getInstance()->setSercetKey(DECODE_SERCET_KEY);
    CPngPacket::getInstance()->setDelegate(sharedImagePacket());
}

void CImagePacket::decodePacket(const char *filePath)
{
    string texName = filePath;
    
    int pos = texName.rfind(".pxs");
    
    if (pos == -1) {
        CCTextureCache::sharedTextureCache()->addImage(filePath);
    }
    else
    {
        texName = texName.substr(0, pos).append(".png");
        
        if (!sharedImagePacket()->reloadImage) {
            
            string fullPathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(texName.c_str());
            
            if (CCTextureCache::sharedTextureCache()->textureForKey(fullPathKey.c_str())) {
                return;
            }
        }
        
        std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(filePath);
        
        unsigned long size = 0;
        unsigned char* pData = CCFileUtils::sharedFileUtils()->getFileData(filePath, "rb", &size);
        
        CPngPacket::getInstance()->decodePacket(pData, size);
        delete [] pData;
    }
}

void CImagePacket::onDecodeSuccessed(string fileName, int pixelFormat, int pixelBit, int width, int height, unsigned char* pixels)
{
    CCTexture2DPixelFormat texPixelFormat = (CCTexture2DPixelFormat)pixelFormat;
 
    string filepath = string("Images/").append(fileName);
    
    if (reloadImage) {

        CCTexture2D* tex = CCTextureCache::sharedTextureCache()->textureForKey(filepath.c_str());
        tex->initWithData(pixels, texPixelFormat, width, height, CCSizeMake(width, height));
    }else{
        
        if (!CCTextureCache::sharedTextureCache()->textureForKey(filepath.c_str())) {
            CCTexture2D* tex = new CCTexture2D();
 
            tex->initWithData(pixels, texPixelFormat, width, height, CCSizeMake(width, height));
            CImageCache::addTexture(filepath, tex);
                                    
            tex->release();
        }
    }
    
}

void CImageCache::addTexture(string key, CCTexture2D *tex)
{
    CImageCache* imageCache = (CImageCache*)CCTextureCache::sharedTextureCache();
    string fullPathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(key.c_str());
    imageCache->m_pTextures->setObject(tex, fullPathKey);
}
