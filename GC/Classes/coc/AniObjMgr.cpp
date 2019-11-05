/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObjMgr.h"
#include "ImgMgr.h"
#include "AniObj_Normal.h"
#include "AniObj_FlyingFont.h"
#include "AniObj_WaitFadeOut.h"
#include "AniObj_Down.h"
#include "AniObj_Move.h"
#include "AniObj_FlyingFont2.h"
#include "AniObj_Flash.h"

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////
//! ActionManager_SpeedUp
ActionManager_SpeedUp::ActionManager_SpeedUp()
	: m_fSpeedUp(1.0f)
{
}

void ActionManager_SpeedUp::update(float dt)
{
	CCActionManager::update(dt * m_fSpeedUp);
}

/////////////////////////////////////////////////////////////////////////
//! AniObjMgr
AniObjMgr& AniObjMgr::getSingleton()
{
	static AniObjMgr s_mgr;

	return s_mgr;
}

AniObjMgr::AniObjMgr()
{
	//load("aniobj.csv");

	m_amgrSpeed = new ActionManager_SpeedUp;
	m_amgrSpeed->retain();
}

AniObjMgr::~AniObjMgr()
{
	m_amgrSpeed->release();
	delete m_amgrSpeed;
}

AniObj* AniObjMgr::newAniObj(CCNode* pParent, const char* name, int dir, bool isAutoRelease, bool isLoop, float maxTime, int zorder, bool inscene, int* oparam)
{
	_Map::const_iterator it = m_map.find(name);
	if(it == m_map.end())
		return NULL;

	AniObj* pObj = NULL;

	if(isLoop)
	{
		pObj = findAniObj(pParent, name);

		if(pObj != NULL)
		{
			if(maxTime > 0)
			{
				if(pObj->m_timeMax < 0)
					pObj->m_timeMax = maxTime;
				else
					pObj->m_timeMax += maxTime;
			}

			return pObj;
		}
	}

    char aniname[128];
    sprintf(aniname, "%s_%d", it->second.aniname.c_str(), dir);
    
	CCActionManager* amgr;

	if(inscene)
		amgr = m_amgrSpeed;
	else
		amgr = NULL;

	switch(it->second.type)
	{
	case ANIOBJ_TYPE_NORMAL:
		{
			pObj = AniObj_Normal::create(aniname, it->second.iParam[0], it->second.iParam[1] / 1000.0f, it->second.iParam[2] / 100.0f, isLoop, amgr);
		}
		break;
	case ANIOBJ_TYPE_FLYINGFONT:
		{
			pObj = AniObj_FlyingFont::create(aniname, it->second.iParam[0], it->second.iParam[1], amgr);
		}
		break;
	case ANIOBJ_TYPE_WAITFADEOUT:
		{
			pObj = AniObj_WaitFadeOut::create(aniname, it->second.iParam[0], it->second.iParam[1], it->second.iParam[2], amgr);
		}
		break;
	case ANIOBJ_TYPE_DOWN:
		{
			pObj = AniObj_Down::create(aniname,
				it->second.iParam[0], it->second.iParam[1] / 1000.0f, it->second.iParam[2],
                it->second.iParam[3], it->second.iParam[4], amgr);
		}
		break;
	case ANIOBJ_TYPE_MOVE:
		{
			pObj = AniObj_Move::create(aniname, it->second.iParam[0], it->second.iParam[1] / 1000.0f,
				oparam[0], oparam[1], oparam[2], oparam[3], oparam[4] / 1000.0f, oparam[5] != 0, amgr);
		}
		break;
	case ANIOBJ_TYPE_FLYINGFONT2:
		{
			pObj = AniObj_FlyingFont2::create(it->second.iParam[0], it->second.iParam[1], amgr);
		}
		break;
	case ANIOBJ_TYPE_FLASH:
		{
			pObj = AniObj_Flash::create(aniname, it->second.iParam[0], it->second.iParam[1] / 1000.0f, it->second.iParam[2] / 100.0f, isLoop, it->second.iParam[3] / 1000.0f, amgr);
		}
		break;
	}

	if(pObj != NULL)
	{
		pObj->retain();
		m_lst.push_back(pObj);
        pObj->setParent(pParent, zorder);
		pObj->m_bAutoRelease = isAutoRelease;
		pObj->m_nameAni = name;
		pObj->m_bLoop = isLoop;
		pObj->m_timeMax = maxTime;
	}

	return pObj;
}

void AniObjMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_Pair p;

		p.first = get("name", i);

		p.second.aniname = get("ani_name", i);

		p.second.iParam[0] = atoi(get("int_param1", i));
		p.second.iParam[1] = atoi(get("int_param2", i));
		p.second.iParam[2] = atoi(get("int_param3", i));
		p.second.iParam[3] = atoi(get("int_param4", i));
		p.second.iParam[4] = atoi(get("int_param5", i));
		p.second.iParam[5] = atoi(get("int_param6", i));

		std::string type = get("type", i);

		if(type == "normal")
			p.second.type = ANIOBJ_TYPE_NORMAL;
		else if(type == "flyingfont")
			p.second.type = ANIOBJ_TYPE_FLYINGFONT;
		else if(type == "waitfadeout")
			p.second.type = ANIOBJ_TYPE_WAITFADEOUT;
		else if(type == "down")
			p.second.type = ANIOBJ_TYPE_DOWN;
		else if(type == "move")
			p.second.type = ANIOBJ_TYPE_MOVE;
		else if(type == "flyingfont2")
			p.second.type = ANIOBJ_TYPE_FLYINGFONT2;
		else if(type == "flash")
			p.second.type = ANIOBJ_TYPE_FLASH;
		else
			p.second.type = ANIOBJ_TYPE_NORMAL;

		m_map.insert(p);
	}
}

void AniObjMgr::onIdle(float ot)
{
	for(std::list<AniObj*>::iterator it = m_lst.begin(); it != m_lst.end(); )
	{
		(*it)->onIdle(ot);
		if((*it)->isFinish() && (*it)->m_bAutoRelease)
		{
			AniObj* pObj = *it;

			it = m_lst.erase(it);

			pObj->releaseImp();

			pObj->release();
		}
        else
            ++it;
	}
}

void AniObjMgr::cleanup()
{
	for(std::list<AniObj*>::iterator it = m_lst.begin(); it != m_lst.end(); ++it)
	{
		AniObj* pObj = *it;

		pObj->releaseImp();

		pObj->release();
	}

	m_lst.clear();
}

AniObjInfo AniObjMgr::getAniObjInfo(const char *name)
{
    _Map::const_iterator it = m_map.find(name);
    return it->second;
}

AniObj* AniObjMgr::findAniObj(CCNode* pParent, const char* str)
{
	for(std::list<AniObj*>::iterator it = m_lst.begin(); it != m_lst.end(); ++it)
	{
		AniObj* pObj = *it;
		if(pObj->m_pParent == pParent && pObj->m_nameAni == str)
			return pObj;
	}

	return NULL;
}

void AniObjMgr::releaseAllAni()
{
    for(std::list<AniObj*>::iterator it = m_lst.begin(); it != m_lst.end(); )
	{
        AniObj* pObj = *it;
        
        it = m_lst.erase(it);
        
        pObj->releaseImp();
        pObj->release();
	}
}

void AniObjMgr::releaseAllAni(CCNode* pParent)
{
	for(std::list<AniObj*>::iterator it = m_lst.begin(); it != m_lst.end(); )
	{
		if((*it)->getParent() == pParent)
		{
			AniObj* pObj = *it;

			it = m_lst.erase(it);

			pObj->releaseImp();
			pObj->release();
		}
        else
            ++it;
	}
}

NS_COC_END
