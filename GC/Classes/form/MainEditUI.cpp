//
//  MainEditUI.cpp
//  GameClient
//
//  Created by Li Chao Jun on 14-10-16.
//
//

#include "MainEditUI.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"
#include "SoundMgr.h"
#include "BuildingMgr.h"
#include "EditBuilding.h"

USING_NS_COC;

CCSize EDIT_ITEM_SIZE;

MainEditUI::MainEditUI()
:m_isEraseMode(false)
,m_isEdited(false)
{
    m_plistArray = CCArray::create();
    CC_SAFE_RETAIN(m_plistArray);
}

MainEditUI::~ MainEditUI()
{
    CC_SAFE_RELEASE(m_plistArray);
    m_plistArray = NULL;
}

bool MainEditUI::init()
{
    if (CUINode::init()) {
    
        CCNode* pEditUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("main_edit.ccbi", this);
        this->setContentSize(pEditUI->getContentSize());
        this->addChild(pEditUI);
        
        for (int i = 1; i <= 4; i++) {
            CCDictionary *plistDict = CCDictionary::createWithContentsOfFile(CCString::createWithFormat("Images/building%03d.plist", i)->getCString());
            m_plistArray->addObject(plistDict);
        }
        
        //复制一份原数据
        for(std::list<UserBuildingInfo*>::iterator it = LogicInfo::getSingleton().m_lstBuilding.begin(); it != LogicInfo::getSingleton().m_lstBuilding.end(); ++it) {
            UserBuildingInfo *buildingInfo = new UserBuildingInfo;
            memcpy(buildingInfo, *it, sizeof(UserBuildingInfo));
            CCLOG("%d", (*it)->state);
            m_lstBuildingSrc.push_back(buildingInfo);
        }
        
        m_pDragInfoLabel->setVisible(false);
        
        CCNode* menuNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("main_button.ccbi", this);
        pEditUI->addChild(menuNode);
        
        for (int i = 0; i < menuNode->getChildrenCount(); i ++) {
            CCNode* itemNode = (CCNode*)menuNode->getChildren()->objectAtIndex(i);
            itemNode->setVisible(false);
        }
        
        EDIT_ITEM_SIZE = CCSizeMake(m_pEditView->getContentSize().height, m_pEditView->getContentSize().height);
        
        PlayerScene::getSingleton().addSelectBuildingListener(this);
        
        m_pScrollView = CCScrollView::create(m_pEditView->getContentSize());
        m_pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
        m_pScrollView->setClippingToBounds(false);
        m_pScrollView->setDelegate(this);
        
        m_pBuildingView = (CCLayer*)m_pScrollView->getContainer();
        m_pBuildingView->setContentSize(m_pEditView->getContentSize());
        
        CUINode* pBuildingUI = CUINode::create(m_pBuildingView->getContentSize());
        pBuildingUI->addChild(m_pScrollView);
        pBuildingUI->setTargetTouch(true);
        m_pEditView->addChild(pBuildingUI);
        
        setIsEraseModeOn(false);
            
        return true;
    }
    
    return false;
}

void MainEditUI::onExit()
{
    CUINode::onExit();
}

void MainEditUI::onEnter()
{
    CUINode::onEnter();
}

bool MainEditUI::onNotify(int notifyid, const void* pVal)
{
    return true;
}

void MainEditUI::checkEditSaveEnable()
{
    if (!PlayerScene::getSingleton().m_lstSelectWall.empty()) {
        m_pBtnSave->setEnabled(false);
        return;
    }
    
    if ((PlayerScene::getSingleton().m_pCurBuilding == NULL || (PlayerScene::getSingleton().m_pCurBuilding->m_pTileColor == NULL && PlayerScene::getSingleton().m_pCurBuilding->m_state != BUILDING_CTRL_MOVE)) &&
        m_pBuildingView->getChildrenCount() == 0) {
        m_pBtnSave->setEnabled(true);
    }
    else {
        m_pBtnSave->setEnabled(false);
    }
}

void MainEditUI::menuSaveCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_isEdited) {
        MessageUI::show(MSG_EDIT_SAVE, ViewString::getValue("edit_save_title").c_str(), ViewString::getValue("edit_save_message").c_str());
    }
    else {
        PlayerScene::getSingleton().m_pGameWorld->hideEditUI();
    }
}

void MainEditUI::menuExitCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_isEdited || m_pBuildingView->getChildrenCount() > 0) {
        MessageUI::show(MSG_EDIT_EXIT, ViewString::getValue("edit_exit_title").c_str(), ViewString::getValue("edit_exit_message").c_str());
    }
    else {
        
        PlayerScene::getSingleton().unselectCurBuilding();
        PlayerScene::getSingleton().m_pGameWorld->hideEditUI();
    }
}

void MainEditUI::editSave()
{
    CCNode* pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
    pLoading->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height/2));
    this->addChild(pLoading);
    
    for(std::list<UserBuildingInfo*>::iterator it = LogicInfo::getSingleton().m_lstBuilding.begin(); it != LogicInfo::getSingleton().m_lstBuilding.end(); ++it) {
        if ((*it)->lasttime > 0) {
            (*it)->lasttime -= (getCurrentTime() - (*it)->curtime);
            (*it)->curtime = getCurrentTime();
        }
    }
    
    HttpClient::getSingleton().saveEdit(LogicInfo::getSingleton().m_lstBuilding);
}

void MainEditUI::editExit()
{
    PlayerScene::getSingleton().unselectCurBuilding();
    
    LogicInfo::getSingleton().m_lstBuilding.clear();
    for(std::list<UserBuildingInfo*>::iterator it = m_lstBuildingSrc.begin(); it != m_lstBuildingSrc.end(); ++it) {
        
        if ((*it)->lasttime > 0) {
            (*it)->lasttime -= (getCurrentTime() - (*it)->curtime);
            (*it)->curtime = getCurrentTime();
        }
        LogicInfo::getSingleton().m_lstBuilding.push_back(*it);
    }
    
    PlayerScene::getSingleton().m_pGameWorld->hideEditUI();
    
    PlayerScene::getSingleton().clearScene();
    PlayerScene::getSingleton().initScene();
}

void MainEditUI::menuRemoveAllCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().hideBuildUI();
    
    PlayerScene::_BuildingList::iterator it = PlayerScene::getSingleton().m_lstBuilding.begin();
    for(; it != PlayerScene::getSingleton().m_lstBuilding.end(); ) {
        
        Building* pBuilding = (*it++);
        PlayerScene::getSingleton().moveBuilding2EditList(*pBuilding);
    }
}

void MainEditUI::menuEraseModeCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        PlayerScene::getSingleton().unselectCurBuilding();
        
        PlayerScene::getSingleton().onBuildingSelect(false, NULL);
    }
    
    m_isEraseMode = !m_isEraseMode;
    
    setIsEraseModeOn(m_isEraseMode);
}

void MainEditUI::setIsEraseModeOn(bool isOn)
{
    CCLabelBMFont *label = (CCLabelBMFont*)m_pBtnEraseMode->getChildByTag(1);
    
    if (m_isEraseMode) {
        label->setString(ViewString::getValue("erase_mode_on").c_str());
        
        m_pBtnEraseMode->setBackgroundSpriteFrameForState(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("button_9_4.png"), CCControlStateNormal);
        m_pBtnEraseMode->setBackgroundSpriteFrameForState(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("button_9_4.png"), CCControlStateHighlighted);
    }
    else {
        label->setString(ViewString::getValue("erase_mode_off").c_str());
        
        m_pBtnEraseMode->setBackgroundSpriteFrameForState(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("button_9_0.png"), CCControlStateNormal);
        m_pBtnEraseMode->setBackgroundSpriteFrameForState(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("button_9_1.png"), CCControlStateHighlighted);
    }
    
    PlayerScene::getSingleton().setIsEraseMode(m_isEraseMode);
}

void MainEditUI::menuRemoveCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        
        if (!LogicInfo::getSingleton().hasFreeWorker()) {
            MessageUI::show(MSG_NORMAL, "", ViewString::getSingleton().getValue("msg_noworker2").c_str(), 0, -1);
            return;
        }
        
        Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
        if(pBuilding != NULL)
        {
            LogicInfo::getSingleton().onRemoveOther(*pBuilding);
        }
    }
}

void MainEditUI::onRemoveOther(UserBuildingInfo* pBuildingInfo)
{
    for(std::list<UserBuildingInfo*>::iterator it = m_lstBuildingSrc.begin(); it != m_lstBuildingSrc.end(); ++it) {
        if ((*it)->id == pBuildingInfo->id && (*it)->buildingid > 90000) {
            UserBuildingInfo *buildingInfo = new UserBuildingInfo;
            memcpy(buildingInfo, pBuildingInfo, sizeof(UserBuildingInfo));
           
            m_lstBuildingSrc.erase(it);
            m_lstBuildingSrc.push_back(buildingInfo);
            
            break;
        }
    }
}

void MainEditUI::onRemoveOtherOK(UserBuildingInfo* pBuildingInfo)
{
    for(std::list<UserBuildingInfo*>::iterator it = m_lstBuildingSrc.begin(); it != m_lstBuildingSrc.end(); ++it) {
        if ((*it)->id == pBuildingInfo->id) {
            m_lstBuildingSrc.erase(it);
            break;
        }
    }
}

void MainEditUI::menuRemoveBuildingCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().hideBuildUI();
    
    if (!PlayerScene::getSingleton().m_lstSelectWall.empty()) {
        
        while (PlayerScene::getSingleton().m_lstSelectWall.size() > 0) {

            Building_Wall* pWall = PlayerScene::getSingleton().m_lstSelectWall[0];
            PlayerScene::getSingleton().moveBuilding2EditList(*pWall);
        }
        
        PlayerScene::getSingleton().clearSelectWallLine();
    }
    else {
        if (PlayerScene::getSingleton().m_pCurBuilding) {
            PlayerScene::getSingleton().moveBuilding2EditList(*PlayerScene::getSingleton().m_pCurBuilding);
        }
    }
}

void MainEditUI::menuFastCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        LogicInfo::getSingleton().onFastBuild();
    }
}

void MainEditUI::playerEffect(int i)
{
    if(i == 302)
    {
        SoundMgr::getSingleton().playEffect("select_barrack.mp3");
    }
    else if(i == 206)
    {
        SoundMgr::getSingleton().playEffect("select_gold.mp3");
    }
    else if(i == 204)
    {
        SoundMgr::getSingleton().playEffect("select_oil.mp3");
    }
    else if(i > 200 && i < 300)
    {
        SoundMgr::getSingleton().playEffect("select_build.mp3");
    }
    else if(i > 100 && i < 200)
    {
        SoundMgr::getSingleton().playEffect("select_attack.mp3");
    }
}

void MainEditUI::onSelectRow(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().hideArrow();
    
    PlayerScene::getSingleton().selectWallLine();
    
    while(!m_lstBuildingBtn.empty())
    {
        m_lstBuildingBtn[m_lstBuildingBtn.size() - 1]->setVisible(false);
        m_lstBuildingBtn.pop_back();
    }
    m_lstBuildingBtn.push_back(m_pBtnRemoveBuilding);
    m_lstBuildingBtn.push_back(m_pBtnRotate);
    
    onRefurbishBuildingBtn();
}

void MainEditUI::onSelectRotate(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().rotationSelectWallLine();
}

void MainEditUI::onRefurbishBuildingBtn()
{
	if(m_lstBuildingBtn.empty())
		return;
    
	int nums = m_lstBuildingBtn.size();
	int sw = CCDirector::sharedDirector()->getWinSize().width;
	int bw = m_lstBuildingBtn[0]->getContentSize().width;
	int ox;
    
	if(nums == 1)
		ox = (sw - bw) / 2 + bw/2;
	else
		ox = (sw - bw * nums - (nums - 1) * 10) / 2 + bw/2;
    
	for(int i = 0; i < nums; ++i)
	{
        m_lstBuildingBtn[i]->setOpacity(0);
		m_lstBuildingBtn[i]->setVisible(true);
        
		m_lstBuildingBtn[i]->setPosition(ccp(ox, 200));
        
        CCFadeIn* fi1 = CCFadeIn::create(0.5f * (i + 1) * 0.3f);
        CCActionInterval* action1 = CCSequence::create(fi1, NULL);
        
        CCMoveTo* mt1 = CCMoveTo::create(0.0f, ccp(ox, 200));
        CCMoveTo* mt2 = CCMoveTo::create(0.3f * (i + 1) * 0.2f, ccp(ox, 260));
        CCMoveTo* mt3 = CCMoveTo::create(0.1f, ccp(ox, 240));
        
        CCActionInterval* action2 = CCSequence::create(mt1, mt2, mt3, NULL);
        
        m_lstBuildingBtn[i]->runAction(action1);
        m_lstBuildingBtn[i]->runAction(action2);
        
		ox += bw + 10;
	}
    
    GuideMgr::getSingleton().refreshArrowPos();
}

bool MainEditUI::onSelect(bool isSelect, Building* pBuilding)
{
    for (int i = 0; i< m_lstBuildingBtn.size(); i++) {
        m_lstBuildingBtn[i]->setVisible(false);
    }
    
    m_lstBuildingBtn.clear();
    
    CCControlButton* pBtnRemove = NULL;
    
	if(isSelect)
	{
        UserBuildingInfo* pUBI = pBuilding->m_pBuildingInfo;
        
#if(COC_CLAN == 0)
        if(pUBI->buildingid / 100 == 305)
            return false;
#endif
        if (pUBI->state == BUILDING_STATE_BUILD || pUBI->state == BUILDING_STATE_UPGRADING)
         {
            if(pUBI->buildingid > 90000)
            {
                m_lstBuildingBtn.push_back(m_pBtnFast);
            }
            else
            {
                m_lstBuildingBtn.push_back(m_pBtnRemoveBuilding);
            }
        }
        else if (pUBI->state == BUILDING_STATE_NORMAL)
        {
            if(pUBI->buildingid > 90000)
            {
                pBtnRemove = m_pBtnRemoveByGold;
                m_lstBuildingBtn.push_back(m_pBtnRemoveByGold);
            }
            else {
                
                m_lstBuildingBtn.push_back(m_pBtnRemoveBuilding);
                
                if(pUBI->buildingid >= 10300 && pUBI->buildingid <= 10399)
                {
                    if(PlayerScene::getSingleton().canSelectWallLine())
                    {
                        m_lstBuildingBtn.push_back(m_pBtnRow);
                    }
                }
            }
        }
        
        playerEffect(pUBI->buildingid / 100);
        
        onRefurbishBuildingBtn();
        
        if(pBtnRemove != NULL)
        {
            char str[128];
            
            BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
            sprintf(str, "%d", (int)info.removecost);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)pBtnRemove->getChildByTag(1);
            pLabelCost->setString(str);
            
            if(info.buildres == MONEY_GOLD)
            {
                if(info.removecost > LogicInfo::getSingleton().m_myInfo.goldex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
            }
            else if(info.buildres == MONEY_ELIXIR)
            {
                if(info.removecost > LogicInfo::getSingleton().m_myInfo.oilex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
            }
        }
        else if(m_pBtnFast->isVisible() == true)
        {
            char str[128];
            
            int i = PlayerScene::getSingleton().m_pCurBuilding->m_lasttime / 240;
            if(i < 1)
                i = 1;
            
            sprintf(str, "%d", i);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnFast->getChildByTag(1);
            pLabelCost->setString(str);
        }
	}
    
	return true;
}

void MainEditUI::addBuilding2EditList(Building* pBuilding)
{
    if(m_pRemoveAllArrowSprite->isVisible()) {
        m_pRemoveAllArrowSprite->setVisible(false);
        m_pRemoveAllInfoLabel->setVisible(false);
        if (m_pDragInfoLabel) {
            m_pDragInfoLabel->setVisible(true);
        }
    }
    
    BuildingInfo info = pBuilding->m_info;
    m_lstBuilding.push_back(pBuilding->m_pBuildingInfo);
    
    EditBuilding *pEditBuilding = getEditBuilding(info.buildingid);
    
    if (pEditBuilding) {
        
        if (pEditBuilding->isVisible()) {
            pEditBuilding->chgCount(1);
        }
    }
    else {
        
        pEditBuilding = EditBuilding::create(info, EDIT_ITEM_SIZE);

        pEditBuilding->chgCount(1);
        pEditBuilding->setTouchControlView(m_pScrollView);
        pEditBuilding->setTag(info.buildingid);
        
        pEditBuilding->setPosition(EDIT_ITEM_SIZE.width * m_pBuildingView->getChildrenCount(), 0);
        
        m_pBuildingView->addChild(pEditBuilding);
        
        layoutListWithEditBuilding(pEditBuilding);
    }
    
    m_pBtnSave->setEnabled(false);
}

void MainEditUI::layoutListWithEditBuilding(EditBuilding *pEditBuilding)
{
    int count = m_pBuildingView->getChildrenCount();
    
    if (!pEditBuilding->isVisible() && count > 0) {
        for (int i = m_pBuildingView->getChildren()->indexOfObject(pEditBuilding)+1; i < m_pBuildingView->getChildrenCount(); i++)
        {
            EditBuilding *ed = (EditBuilding*)m_pBuildingView->getChildren()->objectAtIndex(i);
            ed->setPosition(ccp(ed->getPosition().x-EDIT_ITEM_SIZE.width, ed->getPosition().y));
        }
        
        pEditBuilding->setVisible(false);
        count -= 1;
    }
    
    m_pBuildingView->setContentSize(CCSizeMake(EDIT_ITEM_SIZE.width * count, m_pBuildingView->getContentSize().height));

    if (m_pBuildingView->getContentSize().width > m_pScrollView->getViewSize().width) {
        
        float offsetX = m_pScrollView->getContentOffset().x;
        
        if (offsetX < m_pScrollView->getViewSize().width - m_pBuildingView->getContentSize().width) {
            
            offsetX = m_pScrollView->getViewSize().width - m_pBuildingView->getContentSize().width;
            
            m_pScrollView->setContentOffset(ccp(offsetX, m_pScrollView->getContentOffset().y));
        }
    }

}

void MainEditUI::removeBuildingFromEditList(EditBuilding *pEditBuilding)
{
    if (pEditBuilding->m_isRelease) {
        return;
    }
    
    pEditBuilding->m_isRelease = true;
    pEditBuilding->removeFromParentAndCleanup(true);
}

EditBuilding* MainEditUI::getEditBuilding(int buildingid)
{
    return (EditBuilding*)m_pBuildingView->getChildByTag(buildingid);
}

int MainEditUI::getBuildingCount(int buildingid)
{
    EditBuilding *pEditBuilding = (EditBuilding*)m_pBuildingView->getChildByTag(buildingid);
    
    if (pEditBuilding) {
        return pEditBuilding->m_count;
    }
    
    return 0;
}

Building* MainEditUI::buildWall()
{
    Building_Wall* pBuilding = NULL;
    
    if (PlayerScene::getSingleton().m_pCurBuilding != NULL && PlayerScene::getSingleton().m_pCurBuilding->m_type == BUILDING_TYPE_WALL) {
        
        Building_Wall* pWall = (Building_Wall*)PlayerScene::getSingleton().m_pCurBuilding;
        
        int x = ((PlayerScene::getSingleton().m_ptTouchBegin.x + PlayerScene::getSingleton().m_ptTouchOff.x) - PlayerScene::getSingleton().m_pRoot->getPositionX()) / PlayerScene::getSingleton().getScale();
        
        int y = ((PlayerScene::getSingleton().m_ptTouchBegin.y + PlayerScene::getSingleton().m_ptTouchOff.y) - PlayerScene::getSingleton().m_pRoot->getPositionY()) / PlayerScene::getSingleton().getScale();
        
        CCPoint pt = PlayerScene::getSingleton().countTilePos(x, y);
        
        for (int i = 0; i < 8; i ++) {
            if (pWall->m_pWallEdit[i] && pWall->m_pWallEdit[i]->isIn(pt.x, pt.y)) {
                
                int lx = pWall->m_pWallEdit[(i/2) * 2]->getLogicX();
                int ly = pWall->m_pWallEdit[(i/2) * 2]->getLogicY();
                
                pBuilding = (Building_Wall*)buildBuilding(pWall->m_buildingid, lx, ly);
                
                if (pBuilding) {
                    pBuilding->chgSelMode(true);
                    pBuilding->m_isEditMove = true;
                    
                    EditBuilding *pEditBuilding = getEditBuilding(pBuilding->m_buildingid);
                    if (pEditBuilding->m_count <= 0) {
                        removeBuildingFromEditList(pEditBuilding);
                    }
                }
            }
        }
    }
    
    return pBuilding;
}

Building* MainEditUI::buildBuilding(int buildingid, int lx, int ly)
{
    Building* pBuilding = NULL;
    
    EditBuilding *pEditBuilding = getEditBuilding(buildingid);
    if (pEditBuilding && pEditBuilding->m_count > 0) {
        
        for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it) {
            if ((*it)->buildingid == buildingid) {
                
                pBuilding = PlayerScene::getSingleton().addBuildingEx((*it)->buildingid, *it);
                pBuilding->setLogicPos(lx, ly);
                
                int fulltime = ((pBuilding->m_info.buildtime[0] * 24 + pBuilding->m_info.buildtime[1]) * 60 + pBuilding->m_info.buildtime[2]) * 60;
                if ((*it)->lasttime > 0) {
                    (*it)->lasttime -= (getCurrentTime() - (*it)->curtime);
                    (*it)->curtime = getCurrentTime();
                }
                pBuilding->setTime(fulltime, (*it)->lasttime);
                
                pBuilding->chgState(BUILDING_CTRL_MOVE);
                
                PlayerScene::getSingleton().setBuildingTouchMode(*pBuilding, true);
                PlayerScene::getSingleton().hideBuildUI();
                
                m_lstBuilding.erase(it);
                pEditBuilding->chgCount(-1);
                
                break;
            }
        }
        
        if (m_pDragInfoLabel) {
            m_pDragInfoLabel->removeFromParentAndCleanup(true);
            m_pDragInfoLabel = NULL;
        }
    }
    
    return pBuilding;
}

void MainEditUI::scrollViewDidScroll(CCScrollView* view)
{
    for (int i = 0; i < m_pBuildingView->getChildrenCount(); i++) {
        EditBuilding *pEditBuilding = (EditBuilding*)m_pBuildingView->getChildren()->objectAtIndex(i);
        pEditBuilding->scrollViewDidScroll(view);
    }
}

void MainEditUI::scrollViewDidZoom(CCScrollView* view)
{
}

bool MainEditUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RemoveAllArrowSprite", CCSprite*, m_pRemoveAllArrowSprite);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RemoveAllInfoLabel", CCLabelBMFont*, m_pRemoveAllInfoLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DragInfoLabel", CCLabelBMFont*, m_pDragInfoLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "EditView", CCLayer*, m_pEditView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSave", CCControlButton*, m_pBtnSave);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEraseMode", CCControlButton*, m_pBtnEraseMode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRemoveByGold", CCControlButton*, m_pBtnRemoveByGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRemoveByElixir", CCControlButton*, m_pBtnRemoveByElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFinish", CCControlButton*, m_pBtnFast);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRotate", CCControlButton*, m_pBtnRotate);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRow", CCControlButton*, m_pBtnRow);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRemoveBuilding", CCControlButton*, m_pBtnRemoveBuilding);
    
    return true;
}

SEL_CCControlHandler MainEditUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSaveCallback", MainEditUI::menuSaveCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuExitCallback", MainEditUI::menuExitCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRemoveAllCallback", MainEditUI::menuRemoveAllCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuEraseModeCallback", MainEditUI::menuEraseModeCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "onSelectRow", MainEditUI::onSelectRow);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "onSelectRotate", MainEditUI::onSelectRotate);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRemoveCallback", MainEditUI::menuRemoveCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRemoveBuildingCallback", MainEditUI::menuRemoveBuildingCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFinishCallback", MainEditUI::menuFastCallback);
    
    return NULL;
}

