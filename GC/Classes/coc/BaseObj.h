#ifndef __COC_BASEOBJ_H__
#define __COC_BASEOBJ_H__

#include "BaseDef.h"

NS_COC_BEGIN

class Projectile2;

class BaseObj
{
public:
	BaseObj(int objType);
	virtual ~BaseObj();
public:
	virtual void setLogicPos(int lx, int ly);

	int getLogicX() const { return m_lx; }

	int getLogicY() const { return m_ly; }

	int getObjType() const { return m_objType; }
	
	virtual void chgHP(int off, bool flyfont = false, bool isReal = false);

	virtual bool isEmptyHP() const { return m_hp <= 0; }
    
	void setHP(int hp) { m_hp = hp; }

	int getHP() const { return m_hp; }

	const CCPoint& getCenterPt() const { return m_ptCenter; }

	void setCenterPt(int cx, int cy) { m_ptCenter.x = cx, m_ptCenter.y = cy; }

	int getLogicWidth() const { return m_lw; }

	void setLogicWidth(int lw) { m_lw = lw; }

	virtual CCPoint getAttackPos() const;

	void addProjectile2(Projectile2& proj);

	void releaseProjectile2();

	void onProcProjectile2(int ot);

	virtual void setVisible(bool bVisible);

	bool isVisible() const { return m_isVisible; }
	//PlayerScene& getScene() { return m_scene; }

	virtual void onProc(int ot);

	//! Buff相关
	//! 调整伤害
	int fixDamage(int damage);
	//! 调整速度
	int fixSpeed(int speed);
	//! 调整攻速
	int fixAttspeed(int attspeed);
	//! 调整防御（减伤）
	int fixHP(int off);

	//! 取生命上限
	virtual int getMaxHP() = 0;
	//! 改变生命上限
	void onChgMaxHP();
	//! 调整生命上限
	int fixMaxHP();

	//! 是否是隐身状态
	bool isHidden() { return m_Buffs[BUFF_HIDDEN].value != 0; }

	void addBuff(int type, int value, int time);
	void clearAllBuff();

	//! 添加天赋
	void addTalent(int type, int value, int prob);
	//! 减少天赋
	void removeTalent(int type);

	//! 增加受击动画
	virtual void addEffAni(const char* name);

	virtual int getWidth() = 0;
	virtual int getHeight() = 0;

	void addFlyingFont2(const char* str, int size, ccColor3B c);

	//! 虚函数，载入资源用
	virtual void onLoadRes() {}
	//! 虚函数，释放资源用
	virtual void onFreeRes() {}

	virtual void releaseFromScene();
protected:

	int							m_lx;
	int							m_ly;

	int							m_objType;

	int							m_hp;

	CCPoint						m_ptCenter;

	int							m_lw;

	std::list<Projectile2*>		m_lstProj;

	bool						m_isVisible;

	CCLabelBMFont*				m_pLabelName;

public:
	CCNode*						m_pSkillEffect[3];		// 技能特效 0地表 1身体 2顶层
	BuffNode					m_Buffs[BUFF_MAXNUM];	// 携带的Buff情况
	TalentNode					m_Talents[BUFF_MAXNUM];	// 天赋情况
	int							m_iSecondTime;			// 用于计算秒

	float						m_yTop;					// y轴，血条的高度
    
    int                         m_beAttackTargetNums;

};

NS_COC_END

#endif  // __COC_BASEOBJ_H__