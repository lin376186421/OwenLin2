//
//  CUIExt.h
//  GameClient
//
//  Created by qsun on 13-10-17.
//
//

#ifndef __GameClient__CUIExt__
#define __GameClient__CUIExt__

#include "CUIExt.h"
#include "NGNKit.h"

#include "cocos2d.h"
#include "cocos-ext.h"
#include "../coc/ImgMgr.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_COC;

#define BASE_VIEW_SIZE CCSizeMake(960, 640)

//#define IsRectContains(rect1_, rect2_)  \
//({ \
//bool ret = false; \
//if (rect2_.origin.x >= rect1_.origin.x && rect2_.origin.y >= rect1_.origin.y &&    \
//rect2_.origin.x + rect2_.size.width <= rect1_.origin.x + rect1_.size.width &&  \
//rect2_.origin.y + rect2_.size.height <= rect1_.origin.y + rect1_.size.height)  \
//{  \
//ret = true;  \
//}   \
//ret; \
//})

inline bool IsRectContains(const CCRect& rect1_, const CCRect& rect2_)
{
	if (rect2_.origin.x >= rect1_.origin.x && rect2_.origin.y >= rect1_.origin.y && 
		rect2_.origin.x + rect2_.size.width <= rect1_.origin.x + rect1_.size.width &&
		rect2_.origin.y + rect2_.size.height <= rect1_.origin.y + rect1_.size.height)
	{
		return true;
	}
	return false;
}

static void changeProgress(CCScale9Sprite* pSprite, float progress)
{
    if (pSprite->getPreferredSize().width < pSprite->getContentSize().width) {
        pSprite->setPreferredSize(pSprite->getContentSize());
    }
    if (progress > 1.0) progress = 1.0;
    if (progress <= 0) pSprite->setVisible(false);
    else
    {
        pSprite->setVisible(true);
        
        if (pSprite->getPreferredSize().width*progress < pSprite->getOriginalSize().width/2)
        {
            pSprite->setContentSize(CCSizeMake(pSprite->getOriginalSize().width/2, pSprite->getContentSize().height));
        }
        else
        {
            pSprite->setContentSize(CCSizeMake(pSprite->getPreferredSize().width*progress, pSprite->getContentSize().height));
        }
    }
}

static void changeProgress(CCSprite* pSprite, float progess)
{
    pSprite->setScaleX(progess);
}

static void change2Progress(CCScale9Sprite* pSprite, float progress)
{
    changeProgress(pSprite, progress);
}

static float getResolutionScale()
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float scaleX = winSize.width / BASE_VIEW_SIZE.width;
    float scaleY = winSize.height / BASE_VIEW_SIZE.height;
    
    return MIN(scaleX, scaleY);
}

static CCAction* actionWithAnimate(const char* aniname, int frame, float delay, bool isLoop)
{
    CCArray* animFrames = CCArray::create();
    for (int i = 0; i < frame; i ++) {
        
        char strname[32];
        sprintf(strname, "%s%d", aniname, i);
        CCSpriteFrame* animFrame = ImgMgr::getSingleton().getFrame(strname);
        animFrames->addObject(animFrame);
    }
    
    CCAnimate* pAnimate = CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, delay));
    
    if (isLoop) {
        return CCRepeatForever::create(pAnimate);
    }
    else {
        return pAnimate;
    }
}

static std::string formatTimeAgo(int64_t time, bool isSecond = true)
{
    char str[128];
    
    time = getCurrentTime() - time;
    
    int s = time % 60;
	int m = time % (60 * 60) / 60;
	int h = time % (60 * 60 * 24) / (60 * 60);
    int d = time / (60 * 60 * 24);
    
    char sd[128];
    sprintf(sd, "%dd ", d);
    char sh[128];
    sprintf(sh, "%dh ", h);
    char sm[128];
    sprintf(sm, "%dm ", m);
    char ss[128];
    sprintf(ss, "%ds ", s);
    
    str[0] = '\0';
    if(d > 0)
        strcat(str, sd);
    if(h > 0)
        strcat(str, sh);
    if(d == 0 && m > 0)
        strcat(str, sm);
    if((d == 0 && h == 0) && s > 0 && isSecond)
        strcat(str, ss);
    
    if (str[0] == '\0') {
        return "";
    }
    else {
        string tm = strcat(str, "ago");
        return tm;
    }
}

static std::string formatTimeMs(int64_t ms, int length = 2, bool isSecond = true)
{
    char str[128];
    
    int time = ms / 1000;
    
    int s = time % 60;
	int m = time % (60 * 60) / 60;
	int h = time % (60 * 60 * 24) / (60 * 60);
    int d = time / (60 * 60 * 24);
    
    char sd[128];
    sprintf(sd, "%dd ", d);
    char sh[128];
    sprintf(sh, "%dh ", h);
    char sm[128];
    sprintf(sm, "%dm ", m);
    char ss[128];
    sprintf(ss, "%ds", s);
    
    str[0] = '\0';
    if(d > 0)
        strcat(str, sd);
    if(h > 0)
        strcat(str, sh);
    if(d == 0 && m > 0)
        strcat(str, sm);
    if(((d == 0 && h == 0) || (length == 3 && d == 0)) && s > 0 && isSecond)
        strcat(str, ss);
    
    return std::string(str);
}

static std::string formatTime(int64_t tm, int length = 2, bool isSecond = true)
{
    return formatTimeMs(tm * 1000, length, isSecond);
}

static void setControlButtonFrameForAllState(CCControlButton* pControl, CCSpriteFrame* pFrame)
{
    if (pControl)
    {
        pControl->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
        pControl->setBackgroundSpriteFrameForState(pFrame, CCControlStateHighlighted);
        pControl->setBackgroundSpriteFrameForState(pFrame, CCControlStateDisabled);
    }
}

class CProgress : public CCNode{
    
public:
    
    enum ActionTag{
        ACTION_PROGRESS_FADE_OUT = 1,
    };
    
    CProgress(CCSprite* pProgress, CCSprite* pProgressBg)
    {
        m_pProgress = pProgress;
        m_pProgressBg = pProgressBg;
        
        m_textureRect = m_pProgress->getTextureRect();
        this->setContentSize(m_pProgress->getContentSize());
    }
    
    bool init()
    {
        if (m_pProgressBg) {
            m_pProgressBg->setAnchorPoint(ccp(0, 0.5));
            this->addChild(m_pProgressBg);
        }
        
        m_pProgress->setAnchorPoint(ccp(0, 0.5));
        this->addChild(m_pProgress);
        
        return true;
    }
    
public:
    
    static CProgress* create(CCSprite* pProgress, CCSprite* pProgressBg)
    {
        CProgress *pRet = new CProgress(pProgress, pProgressBg);
        if (pRet && pRet->init())
        {
            pRet->autorelease();
            return pRet;
        }
        else
        {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    
    void changeProgress(float progress)
    {
        if (progress > 1.0) progress = 1.0;
        if (progress < 0) progress = 0;
        
        CCRect rect(m_textureRect.origin.x, m_textureRect.origin.y, this->getContentSize().width * progress, m_textureRect.size.height);
        m_pProgress->setTextureRect(rect, m_pProgress->isTextureRectRotated(), rect.size);
    }
    
    void setPreferredSize(CCSize size)
    {
        //        m_pProgress-> (m_preferredSize);
        //        if (m_pProgressBg) {
        //            m_pProgressBg->setPreferredSize(m_preferredSize);
        //        }
        //
        //        this->setContentSize(m_preferredSize);
    }
    
    void addFadeOut(int time)
    {
        m_pProgress->stopActionByTag(ACTION_PROGRESS_FADE_OUT);
        m_pProgressBg->stopActionByTag(ACTION_PROGRESS_FADE_OUT);
        
        CCFadeOut* fadeOut = CCFadeOut::create(time);
        fadeOut->setTag(ACTION_PROGRESS_FADE_OUT);
        m_pProgress->runAction(fadeOut);
        fadeOut = CCFadeOut::create(time);
        fadeOut->setTag(ACTION_PROGRESS_FADE_OUT);
        m_pProgressBg->runAction(fadeOut);
    }
    
public:
    CCSprite* m_pProgressBg;
    CCSprite* m_pProgress;
    
    CCRect  m_textureRect;
};

#endif /* defined(__GameClient__CUIExt__) */
