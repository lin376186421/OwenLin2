/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_IMGMGR_H__
#define __ZEPHYR_IMGMGR_H__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class ImgMgr : public BaseCSV, public CCObject
{
	struct _ImgInfo{
		std::string				plist;
		std::string				file;
        std::string             strTex;
		CCTexture2DPixelFormat	format;

		CCTexture2D*			pTex;
	};

	typedef std::pair<std::string, _ImgInfo>	_Pair;
	typedef std::map<std::string, _ImgInfo>		_Map;
public:
	static ImgMgr& getSingleton();
protected:
	ImgMgr();
	virtual ~ImgMgr();
public:
	void release();

	CCSpriteFrame* getFrame(const char* name);

	void startAsyncLoad(CCObject* pTarget, SEL_CallFunc func);

	void onAsyncLoadFinish();
    
	void addAsyncImg(const char* name);

    CCSprite* loadSprite(const char* name);
    
    CCTexture2D* loadImg(const char* name);
	void addSpriteFrames(const char* name);

	void removeRes(const char* name);
    void removeAllRes();
    
    void reloadAllImage();
    
	void onAsyncLoad(float ot);

	CCSpriteBatchNode* addBatchNode(const char* name);
public:
	virtual void onLoadEnd();
public:
	std::list<std::string>	m_lstImg;
	int						m_numsLoaded;
	int						m_lastLoadFrames;
	bool					m_isAsyncStart;

	SEL_CallFunc			m_pFuncLoadEnd;
	CCObject*				m_pTargetLoadEnd;

	_Map					m_map;
};

NS_COC_END

#endif  // __ZEPHYR_IMGMGR_H__