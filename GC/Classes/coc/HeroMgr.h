//
//  HeroMgr.h
//  GameClient
//
//  Created by chowray on 13-9-23.
//
//

#ifndef __GameClient__HeroMgr__
#define __GameClient__HeroMgr__

#include "BaseDef.h"

NS_COC_BEGIN

class Building;

enum HeroState {
    HERO_STATE_NORMAL = 0,
    HERO_STATE_UPGRADING = 1,
};
 
class HeroMgr : CCObject
{
public:
    HeroMgr();
    ~HeroMgr();
public:
	static HeroMgr& getSingleton();
    
    void init();

    void createHero(int heroid);
    
	void onProcHero(int ot);
    
    int onProcHeroHp();

	int getHeroLastTime();
    int getFillHeroHPGems();

	void onBeginAttack();

    bool isFullHP();
	bool fastHeroHP();

	int getRecoverHP();

	int countWarExp(int star, int atkbaselevel, int defbaselevel);

	bool canLevelUp();

	int getHeroLevel();
    
    int upLevelHero(int &need);
    
    int getEquSkill(int index);
    
    bool finishUpgrade(bool fast);
    
    void cancelUpgrade();
    
    void refreshHero();
    
    void upgradeActionCall();
    
    void onUpdateHeroTime();
        
    void setTime(int fulltime, int lasttime);

public:
    HeroInfo2		m_myHero;
    
	HeroInfo2		m_otherHero;
    
    FightCtrlNode   m_otherHeroCtrl;

	int					m_curAttackExp;

	int					m_curAttackHPOff;

	bool				m_isHeroInWar;

	int					m_lbnums;
	int					m_cgnums;
    
    bool                m_isSelect;
    
    int                 m_heroState;
    
    int                 m_otherHeroState;
    
    int                 m_isFreeTrain;
    
    
    int                 m_heroCurTime;
    
    int                 m_heroFullTime;
    
    int                 m_heroLastTime;
    
    int                 m_heroLastTimems;
    
    time_t              m_lastHeroHPTime;
    
    CCAction*          m_pUpgradeAction[2];
    
    bool                m_showLightAction;
};

NS_COC_END

#endif /* defined(__GameClient__HeroMgr__) */
