/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "ImgMgr.h"
#include "CImagePacket.h"
#include "NGNKit.h"
#include "ccFPSImages.h"

NS_COC_BEGIN

#define PNG_ENCODE 1

ImgMgr& ImgMgr::getSingleton()
{
	static ImgMgr s_mgr;

	return s_mgr;
}

ImgMgr::ImgMgr()
	: m_pFuncLoadEnd(NULL)
	, m_pTargetLoadEnd(NULL)
	, m_lastLoadFrames(10)
	, m_isAsyncStart(false)
{
	
}

ImgMgr::~ImgMgr() 
{
	release();
}

void ImgMgr::release()
{
	//if(m_pCacheImg != NULL)
	//	m_pCacheImg->release();

	//m_mapImgInfo.clear();

	//BaseCSV::release();
}

void ImgMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_Pair p;

		p.first = get("imgname", i);

		p.second.plist = get("plistname", i);
        
        p.second.file = get("imgfile", i);
        
        p.second.strTex = CCStringFormat("Images/%s.png", p.first.c_str());
        
		p.second.pTex = NULL;

		std::string format = get("format", i);

		if(format == "rgba4444")
			p.second.format = kCCTexture2DPixelFormat_RGBA4444;
		else if(format == "rgb565")
			p.second.format = kCCTexture2DPixelFormat_RGB565;
		else if(format == "rgba8888")
			p.second.format = kCCTexture2DPixelFormat_RGBA8888;
		else if(format == "rgb888")
			p.second.format = kCCTexture2DPixelFormat_RGB888;
		else if(format == "pvrtc4")
			p.second.format = kCCTexture2DPixelFormat_PVRTC4;
		else
			p.second.format = kCCTexture2DPixelFormat_RGBA8888;

		m_map.insert(p);
	}
}

CCSpriteFrame* ImgMgr::getFrame(const char* name)
{
#ifdef IMGMGR_NAME_NOPNG
	char strname[128];
	sprintf(strname, "%s.png", name);

	CCSpriteFrame* pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(strname);
#else // IMGMGR_NAME_NOPNG
	CCSpriteFrame* pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
#endif // IMGMGR_NAME_NOPNG

    if (pFrame == NULL) {
        CCTexture2D* pTex = loadImg(name);
        
        if (pTex != NULL) {
            
            CCRect rect = CCRectZero;
            rect.size = pTex->getContentSize();
            
            pFrame = CCSpriteFrame::createWithTexture(pTex, rect);
        }
    }    
	return pFrame;
}

CCSprite* ImgMgr::loadSprite(const char* name)
{
    CCSpriteFrame* pSpriteFrame = getFrame(name);
    if (pSpriteFrame) {
        CCSprite* pSpr = CCSprite::createWithSpriteFrame(pSpriteFrame);
        
        return pSpr;
    }
    
    return NULL;
}

CCTexture2D* ImgMgr::loadImg(const char* name)
{
#ifdef IMGMGR_NAME_NOPNG
	char strname[128];
	sprintf(strname, "%s.png", name);

#endif // IMGMGR_NAME_NOPNG

    _Map::iterator it = m_map.find(name);
    if(it == m_map.end())
    {
        return NULL;
    }
    else if (it->second.pTex != NULL) {
        return it->second.pTex;
    }
            
#if PNG_ENCODE
    CImagePacket::decodePacket(it->second.file.c_str());
    it->second.pTex = CCTextureCache::sharedTextureCache()->textureForKey(it->second.strTex.c_str());
#else
    it->second.pTex = CCTextureCache::sharedTextureCache()->addImage(it->second.file.c_str());
#endif
            
    CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    
    return it->second.pTex;
}

void ImgMgr::onAsyncLoadFinish()
{
	m_numsLoaded = 0;

	for(std::list<std::string>::iterator iter = m_lstImg.begin(); iter != m_lstImg.end(); ++iter)
	{
        _Map::iterator it = m_map.find((*iter).c_str());
		if(it->second.pTex != NULL) {
			++m_numsLoaded;
        }
	}
	
	if(m_pTargetLoadEnd != NULL && m_pFuncLoadEnd != NULL)
	{
		(m_pTargetLoadEnd->*m_pFuncLoadEnd)();
	}

	if(m_numsLoaded == m_lstImg.size())
	{
		m_isAsyncStart = false;
	}
}

void ImgMgr::addAsyncImg(const char* name)
{
	_Map::iterator it = m_map.find(name);
	if(it != m_map.end())
	{
		m_lstImg.push_back(name);
	}
}

void ImgMgr::startAsyncLoad(CCObject* pTarget, SEL_CallFunc func)
{
	m_pFuncLoadEnd = func;
	m_pTargetLoadEnd = pTarget;

	m_isAsyncStart = true;

	m_numsLoaded = 0;

	for(std::list<std::string>::iterator iter = m_lstImg.begin(); iter != m_lstImg.end(); ++iter)
	{
        _Map::iterator it = m_map.find((*iter).c_str());
		if(it->second.pTex != NULL)
			++m_numsLoaded;
	}

	if(m_numsLoaded == m_lstImg.size())
	{
		if(m_pTargetLoadEnd != NULL && m_pFuncLoadEnd != NULL)
		{
			(m_pTargetLoadEnd->*m_pFuncLoadEnd)();
		}

		m_isAsyncStart = false;
	}
}

void ImgMgr::addSpriteFrames(const char* name)
{
	_Map::iterator it = m_map.find(name);
	if(it == m_map.end())
		return ;
    CCTexture2DPixelFormat oldPixelFormat = CCTexture2D::defaultAlphaPixelFormat();
	CCTexture2D::setDefaultAlphaPixelFormat(it->second.format);

	if(it->second.plist.empty()){
#if PNG_ENCODE

        CImagePacket::decodePacket(it->second.file.c_str());

		it->second.pTex = CCTextureCache::sharedTextureCache()->textureForKey(it->second.strTex.c_str());
#else
        it->second.pTex = CCTextureCache::sharedTextureCache()->addImage(it->second.file.c_str());
#endif
    }
	else
	{
#if PNG_ENCODE
        CImagePacket::decodePacket(it->second.file.c_str());
        it->second.pTex = CCTextureCache::sharedTextureCache()->textureForKey(it->second.strTex.c_str());

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(it->second.plist.c_str(), it->second.pTex);
#else
        it->second.pTex = CCTextureCache::sharedTextureCache()->addImage(it->second.file.c_str());

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(it->second.plist.c_str(), it->second.file.c_str());
#endif
	}

	//CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    
    CCTexture2D::setDefaultAlphaPixelFormat(oldPixelFormat);
}

void ImgMgr::onAsyncLoad(float ot)
{
	if(!m_isAsyncStart)
		return;

	if(--m_lastLoadFrames > 0)
		return ;

	m_lastLoadFrames = 10;
    
    CCTexture2DPixelFormat oldPixelFormat = CCTexture2D::defaultAlphaPixelFormat();

	for(std::list<std::string>::iterator iter = m_lstImg.begin(); iter != m_lstImg.end(); ++iter)
	{
        _Map::iterator it = m_map.find((*iter).c_str());
		if(it->second.pTex == NULL)
		{
			CCTexture2D::setDefaultAlphaPixelFormat(it->second.format);

#if PNG_ENCODE
            CImagePacket::decodePacket(it->second.file.c_str());
            it->second.pTex = CCTextureCache::sharedTextureCache()->textureForKey(it->second.strTex.c_str());
#else
            it->second.pTex = CCTextureCache::sharedTextureCache()->addImage(it->second.file.c_str());
#endif
            ImgMgr::getSingleton().onAsyncLoadFinish();

			if(!it->second.plist.empty())
				CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(it->second.plist.c_str(), it->second.pTex);

			break ;
		}
	}
    
    CCTexture2D::setDefaultAlphaPixelFormat(oldPixelFormat);
}

CCSpriteBatchNode* ImgMgr::addBatchNode(const char* str)
{
	_Map::const_iterator it = m_map.find(str);
	if(it == m_map.end())
		return NULL;

	if(it->second.pTex == NULL)
		return NULL;

	if(it->second.pTex != NULL)
		return CCSpriteBatchNode::createWithTexture(it->second.pTex);

	return NULL;
}

void ImgMgr::removeRes(const char* name)
{
	_Map::iterator it = m_map.find(name);
	if(it == m_map.end())
		return ;

	it->second.pTex = NULL;

	if(!it->second.plist.empty())
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(it->second.plist.c_str());

#if PNG_ENCODE
	CCTextureCache::sharedTextureCache()->removeTextureForKey(it->second.strTex.c_str());
#else
    CCTextureCache::sharedTextureCache()->removeTextureForKey(it->second.file.c_str());
#endif

	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();

    CCLOG("remove image -> %s", name);
}

void ImgMgr::removeAllRes()
{
    for (_Map::iterator it = m_map.begin(); it != m_map.end(); it++) {
        
        if (it->second.pTex != NULL) {
            removeRes(it->first.c_str());
        }
    }
}

void ImgMgr::reloadAllImage()
{
    CCLOG("reload all image");
    
    CImagePacket::sharedImagePacket()->reloadImage = true;
    for (_Map::iterator it = m_map.begin(); it != m_map.end(); it++) {
        
        if (it->second.pTex != NULL) {
            CImagePacket::decodePacket(it->second.file.c_str());
            CCLOG("reload image -> %s", it->second.strTex.c_str());
        }
    }
    CImagePacket::sharedImagePacket()->reloadImage = false;

    CCTextureCache::reloadAllTextures();
}

NS_COC_END
