//
//  SkillMgr.h
//  GameClient
//
//  Created by chowray on 13-9-23.
//
//

#ifndef __GameClient__SkillMgr__
#define __GameClient__SkillMgr__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class SkillMgr : public BaseCSV
{
public:
	struct _skillNode{
		Person* hero;
		int id;
		const SkillInfo* sinfo;
		int time;
	};

	typedef std::list<_skillNode>		_SkillList;
	
public:
    SkillMgr();
    ~SkillMgr();
public:
	static SkillMgr& getSingleton();
	typedef std::map<int, SkillInfo>			_Map;
	typedef std::pair<int, SkillInfo>			_Pair;
public:
    std::vector<SkillInfo*>	m_lstCard;
	void release();
    
    void init();
    
	const SkillInfo* getNode(int skillid) const;
    
	int chgSkill3(int skillid);

	void onIdle(int ot);

	//! 升级技能
	int upgradeSkill(int& need, int skillId, int type);
	
	//! 释放技能
	void useSkill(Person* hero, int id);

	//! 释放重击
	void useSkill_10006(Person* hero, const SkillInfo* sinfo);
	//! 释放激昂
	void useSkill_10002(Person* hero, const SkillInfo* sinfo);
	//! 释放守护
	void useSkill_10003(Person* hero, const SkillInfo* sinfo);
	//! 释放斗志
	void useSkill_10004(Person* hero, const SkillInfo* sinfo);
	//! 释放强击
	void useSkill_10005(Person* hero, const SkillInfo* sinfo);
	//! 释放治疗
	void useSkill_10001(Person* hero, const SkillInfo* sinfo);
	//! 释放拆除
	void useSkill_10007(Person* hero, const SkillInfo* sinfo);
	//! 释放挑衅
	void useSkill_10008(Person* hero, const SkillInfo* sinfo);
	//! 释放爆发
	void useSkill_10009(Person* hero, const SkillInfo* sinfo);
	//! 释放远视
	void useSkill_10010(Person* hero, const SkillInfo* sinfo);
	//! 释放闪电
	void useSkill_10011(Person* hero, const SkillInfo* sinfo);
	//! 释放隐身
	void useSkill_10012(Person* hero, const SkillInfo* sinfo);


	//! 添加天赋
	void addTalent(Person* hero, int id);

	bool canLevelUp(int skillid);

	//! 获取每次训练的经验
	int getSkillTrainExp(int skillid);
	//! 获取每次训练的消耗
	int getSkillTrainGem(int skillid);
	//! 获取训练到升级的消耗
	int getSkillTotalTrainGem(int skillid);
    
public:
	virtual void onLoadEnd();

	virtual void onRelease();
public:
	_Map		m_mapInfo;
	_SkillList	m_lstSkill;
            
    int        m_skillNums;
};

NS_COC_END


#endif /* defined(__GameClient__SkillMgr__) */
