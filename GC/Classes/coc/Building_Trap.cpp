#include "Building_Trap.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "LogicInfo.h"
#include "Person.h"

NS_COC_BEGIN

Building_Trap::Building_Trap(BuildingInfo info)
	: Building_Attack(info)
	, m_bDestroy(false)
	, m_iAttackTime(0)
{
	m_type = BUILDING_TYPE_TRAP;
}

Building_Trap::~Building_Trap()
{
}

void Building_Trap::init(int buildingid, int lx, int ly)
{
	Building_Attack::init(buildingid, lx, ly);
}

void Building_Trap::setLogicPos(int lx, int ly)
{
	Building_Attack::setLogicPos(lx, ly);
    
    if (!LogicInfo::getSingleton().isNormalMode()) {
        Building_Attack::setVisible(false);
    }
}

void Building_Trap::releaseFromScene()
{
	Building_Attack::releaseFromScene();
}

void Building_Trap::reorder(int zorder)
{
	Building_Attack::reorder(zorder);
}

void Building_Trap::chgSelMode(bool isSel)
{
	Building_Attack::chgSelMode(isSel);
}

int Building_Trap::getHeight()
{
	if(m_pBulding != NULL)
		return m_pBulding->getContentSize().height;

	return Building_Attack::getHeight();
}

void Building_Trap::onBuildingEnd()
{
	Building_Attack::onBuildingEnd();
}

void Building_Trap::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true);
}

void Building_Trap::onProc(int ot)
{
	Building_Attack::onProc(ot);

	if(LogicInfo::getSingleton().isAttackMode())
	{
#ifdef COC_NOBUILDING_ATTACK
		return;
#endif // COC_NOBUILDING_ATTACK

		if(!m_bDestroy)
		{
			if(m_iAttackTime == 0)
			{
				if(m_pDestPerson != NULL)
				{
					if(m_pBulding != NULL)
						m_pBulding->setVisible(true);

					m_iAttackTime = TRAPBOMB_TIME * 1000;
				}
			}
			else
			{
				m_iAttackTime -= ot;

				if(m_iAttackTime <= 0)
				{
					m_iAttackTime = 0;
					m_bDestroy = true;

					if(m_pBulding != NULL)
						m_pBulding->setVisible(false);
                    
                    float bx = m_pBulding->getPositionX();
                    float by = m_pBulding->getPositionY() + m_pBulding->getContentSize().height/2;

                    char atkani[64];
                    sprintf(atkani, "building%d_atk", m_buildingid/100);
                    
                    char hitani[64];
                    sprintf(hitani, "building%d_hit", m_buildingid/100);
                    
                    if (m_buildingid/100 == BUILDING_AIR_BOMB) {
                        
                        if (m_pDestPerson != NULL && !m_pDestPerson->isEmptyHP()) {
                            
                            PlayerScene::getSingleton().addBoomEx(atkani, 0, getAttackPos().x, getAttackPos().y, false);
                            
                            PlayerScene::getSingleton().addProjectile2("building503_bullet", bx, by, m_pDestPerson, 0.02f, "building503_hit", false, false, false, 1500, "", 0.0f, true, false, m_info.damage, CAMP_DEFENCE, 0.0f, false);
                        }
                    }
                    else if (m_buildingid/100 == BUILDING_SPIKE_TRAP) {
                        
                        PlayerScene::getSingleton().addBoomEx(atkani, 0, bx, by, false);
                        PlayerScene::getSingleton().addBoomEx2(hitani, 0, bx, by, m_pBulding->getZOrder());
                                                
                        //! 刺亡
						PlayerScene::getSingleton().procStab(getCenterPt().x, getCenterPt().y, m_info.attackrange / LOGIC_METER, CAMP_ATTACK, m_pBulding->getPositionX(), m_pBulding->getPositionY());
                        
                    }
                    else {
                        
                        PlayerScene::getSingleton().addBoomEx(atkani, 0, bx, by, false);
                        PlayerScene::getSingleton().addBoomEx2(hitani, 0, bx, by, m_pBulding->getZOrder());
                        
                        PlayerScene::getSingleton().procAOE(getCenterPt().x, getCenterPt().y, m_info.damage, m_info.attackrange / LOGIC_METER, CAMP_DEFENCE, 0, false, 0, true);
                    }
                }
			}
		}
	}
}

NS_COC_END