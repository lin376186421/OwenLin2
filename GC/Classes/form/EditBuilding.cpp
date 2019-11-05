//
//  EditBuilding.cpp
//  GameClient
//
//  Created by Li Chao Jun on 14/10/20.
//
//

#include "EditBuilding.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"
#include "BuildingMgr.h"

EditBuilding::EditBuilding()
:m_count(0)
,m_status(EditStatus_Invalid)
,m_isRelease(false)
{
}

EditBuilding::~EditBuilding()
{
    
}

bool EditBuilding::init(BuildingInfo info, CCSize size)
{
    if (!CUINode::init()) {
        return false;
    }
    
    setContentSize(size);
        
    int buildid = info.buildingid / 100;
    
    char str[1024];
    sprintf(str, ViewString::getSingleton().getValue("info_title").c_str(), info.name.c_str(), info.buildingid % 100);
    
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("arms_headbg_0");
    m_button = CCControlButton::create(CCScale9Sprite::createWithSpriteFrame(pFrame));
    m_button->setPreferredSize(size);
    m_button->setZoomOnTouchDown(false);
    m_button->setAnchorPoint(ccp(0.5f, 0.5f));
    m_button->setScale(0.95f);
    CCSize buttonSize(m_button->getPreferredSize().width*m_button->getScaleX(),
                      m_button->getPreferredSize().height*m_button->getScaleY());

    m_button->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
    
    CCNode *node = CCNode::create();
    m_button->addChild(node, 1);
    
    CCSize canvasSize;
    CCRect maxSourceColorRect = CCRectZero;
    
    if(buildid == BUILDING_WALL) {
        sprintf(str,"%s_1_2", info.res.c_str());
        CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
        if (pSprite) {
            canvasSize = pSprite->getContentSize();
            getMaxSourceColorRect(str, maxSourceColorRect);
            node->addChild(pSprite);
        }
    }
    else {
        sprintf(str,"%s_1", info.res.c_str());
        CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
        if (pSprite) {
            canvasSize = pSprite->getContentSize();
            getMaxSourceColorRect(str, maxSourceColorRect);
            node->addChild(pSprite);
        }
        
        sprintf(str,"%s_2", info.res.c_str());
        pSprite = ImgMgr::getSingleton().loadSprite(str);
        if (pSprite) {
            canvasSize = pSprite->getContentSize();
            getMaxSourceColorRect(str, maxSourceColorRect);
            node->addChild(pSprite);
        }
        
        if(buildid == BUILDING_CANNON || buildid == BUILDING_WIZARD_TOWER) {
            sprintf(str,"%s_2_5", info.res.c_str());
            pSprite = ImgMgr::getSingleton().loadSprite(str);
            if (pSprite) {
                canvasSize = pSprite->getContentSize();
                getMaxSourceColorRect(str, maxSourceColorRect);
                node->addChild(pSprite);
            }
        }
        else if (buildid == BUILDING_ELIXIR_STORAGE || buildid == BUILDING_GOLD_STORAGE){
            sprintf(str,"%s_2_0", info.res.c_str());
            
            pSprite = ImgMgr::getSingleton().loadSprite(str);
            if (pSprite) {
                canvasSize = pSprite->getContentSize();
                getMaxSourceColorRect(str, maxSourceColorRect);
                node->addChild(pSprite);
            }
        }
    }
    
    //缩放中间建筑node
    float sx = m_button->getContentSize().width/maxSourceColorRect.size.width;
    float sy = m_button->getContentSize().height/maxSourceColorRect.size.height;
    float s = MIN(sx, sy);
    if (s > 1) {
        s = 1;
    }
    node->setScale(s*0.75f);
    CCPoint anchor = CCPointMake((maxSourceColorRect.origin.x+maxSourceColorRect.size.width/2)/canvasSize.width,
                                 (maxSourceColorRect.origin.y+maxSourceColorRect.size.height/2)/canvasSize.height);
    CCPoint offect = ccpSub(ccp(0.5f, 0.5f), anchor);
    node->setPosition(m_button->getContentSize().width*(0.5f+offect.x),
                      m_button->getContentSize().height*(0.5f+offect.y));
    
    //
    int level = info.buildingid%100;
    char buildingLevel[128];
    sprintf(buildingLevel, ViewString::getSingleton().getValue("edit_building_level").c_str(), level);
    m_levelLabel = CCLabelBMFont::create(buildingLevel, "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
    m_button->addChild(m_levelLabel, 1);
    m_levelLabel->setPosition(m_button->getContentSize().width/2, m_button->getContentSize().height*0.15f);
    m_levelLabel->setScale(0.35f);
    
    m_countLabel = CCLabelBMFont::create("", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
    m_button->addChild(m_countLabel, 1);
    m_countLabel->setPosition(m_button->getContentSize().width/2, m_button->getContentSize().height*0.85f);
    m_countLabel->setScale(0.35f);
    
    addChild(m_button);
    
    m_button->addTargetWithActionForControlEvents(this, cccontrol_selector(EditBuilding::menuTouchDownCallback), 0xFFFFFF);
    
    return true;
}

EditBuilding* EditBuilding::create(BuildingInfo info, CCSize size)
{
    EditBuilding *pRet = new EditBuilding();
    pRet->init(info, size);
    pRet->autorelease();
    return pRet;
}

void EditBuilding::getMaxSourceColorRect(const char* frameName, CCRect &rect)
{
    CCArray *plistArray = PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->m_plistArray;
    for (int i = 0; i <= plistArray->count(); i++) {
        CCDictionary *plistDict = (CCDictionary*)plistArray->objectAtIndex(i);
        CCDictionary *frames = (CCDictionary*)plistDict->objectForKey("frames");
        CCDictionary *frame = (CCDictionary*)frames->objectForKey(CCString::createWithFormat("%s.png", frameName)->getCString());
        if (frame) {
            CCRect frameRect = CCRectFromString(((CCString*)frame->objectForKey("sourceColorRect"))->getCString());
            CCSize sourceSize = CCSizeFromString(((CCString*)frame->objectForKey("sourceSize"))->getCString());
            
            float frameLeft = frameRect.origin.x;
            float frameRight = frameLeft+frameRect.size.width;
            float frameTop = sourceSize.height-frameRect.origin.y;
            float frameBottom = frameTop-frameRect.size.height;
            
            if (rect.equals(CCRectZero)) {
                rect = CCRectMake(frameLeft, frameBottom, frameRight-frameLeft, frameTop-frameBottom);
            }
            else {
                float left = MIN(rect.origin.x, frameLeft);
                float right = MAX(rect.origin.x+rect.size.width, frameRight);
                float top = MAX(rect.origin.y+rect.size.height, frameTop);
                float bottom = MIN(rect.origin.y, frameBottom);
                rect = CCRectMake(left, bottom, right-left, top-bottom);
            }
            break;
        }
    }
}

void EditBuilding::chgCount(int offect)
{
    m_count += offect;
    
    if (m_count == 0) {
        this->setVisible(false);
        PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->layoutListWithEditBuilding(this);
                
        return;
    }
    
    char tmp[32];
    sprintf(tmp, "x%d", m_count);
    if (m_count > 1) {
        m_countLabel->setVisible(true);
        m_countLabel->setString(tmp);
    }
    else {
        m_countLabel->setVisible(false);
    }
}

void EditBuilding::scrollViewDidScroll(CCScrollView* view)
{
    if (m_status == EditStatus_TouchDown || m_status == EditStatus_DragInside) {
        m_status = EditStatus_Scroll;
        m_button->stopAllActions();
        m_button->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
    }
}

void EditBuilding::menuTouchDownCallback(CCObject* pSender, CCControlEvent event)
{
    MainEditUI* pMainEditUI = PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI;
    
    CCPoint worldPoint = PlayerScene::getSingleton().m_pGameWorld->m_pGameLayer->convertToNodeSpace(m_button->getTouchPosition());
    
    switch (event) {
        case CCControlEventTouchDown: {
            m_status = EditStatus_TouchDown;
            m_touchDownPoint = m_button->getTouchPosition();
        } break;
            
        case CCControlEventTouchDragInside: {
            if (m_status != EditStatus_DragOutside) {
                if (m_status == EditStatus_Scroll) {
                    m_button->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
                }
                else {
                    m_status = EditStatus_DragInside;
                    if (m_button->getTouchPosition().y > m_touchDownPoint.y) {
                        float offect = m_button->getTouchPosition().y-m_touchDownPoint.y;
                        if (offect > getContentSize().height/2) {
                            offect = getContentSize().height/2;
                        }
                        m_button->setPosition(ccp(getContentSize().width/2, getContentSize().height/2+offect));
                    }
                }
            }
        } break;
            
        case CCControlEventTouchUpInside: {
            if (m_status == EditStatus_TouchDown) {
                m_button->runAction(CCSequence::createWithTwoActions(CCMoveTo::create(0.1f, ccp(getContentSize().width/2, getContentSize().height)), CCMoveTo::create(0.15f, ccp(getContentSize().width/2, getContentSize().height/2))));
            }
            else if (m_status == EditStatus_DragInside) {
                m_button->runAction(CCMoveTo::create(0.15f, ccp(getContentSize().width/2, getContentSize().height/2)));
            }
            
            pMainEditUI->m_pScrollView->setTouchEnabled(true);
        } break;
            
        case CCControlEventTouchDragOutside: {
            if (m_status != EditStatus_DragOutside) {
                
                if (m_button->getTouchPosition().y > pMainEditUI->m_pEditView->getContentSize().height) {
                    m_status = EditStatus_DragOutside;
                                        
                    int x = (worldPoint.x - PlayerScene::getSingleton().m_pRoot->getPosition().x) / PlayerScene::getSingleton().getScale();
                    int y = (worldPoint.y - PlayerScene::getSingleton().m_pRoot->getPosition().y) / PlayerScene::getSingleton().getScale();
                    
                    int buildingid = getTag();
                    CCPoint pt = PlayerScene::getSingleton().countTilePos(x, y);
                    
                    if (PlayerScene::getSingleton().m_pCurBuilding && PlayerScene::getSingleton().m_pCurBuilding->m_pTileColor) {
                        if (PlayerScene::getSingleton().m_pCurBuilding->m_editStatus == EDITBUILD_STATUS_SUCCESS) {
                            PlayerScene::getSingleton().buildCurBuilding();
                        }
                        else {
                            PlayerScene::getSingleton().moveBuilding2EditList(*PlayerScene::getSingleton().m_pCurBuilding);
                        }
                    }
                    
                    pMainEditUI->buildBuilding(buildingid, pt.x, pt.y);
                    
                    m_beginPos = worldPoint;
                    
                    m_button->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
                    pMainEditUI->m_pScrollView->setTouchEnabled(false);
                }
            }
            
            if (m_status == EditStatus_DragOutside) {
                PlayerScene::getSingleton().onTouchMoved(worldPoint.x-m_beginPos.x, worldPoint.y-m_beginPos.y);
            }
        } break;
            
        case CCControlEventTouchUpOutside: {
            if (m_status == EditStatus_DragOutside) {
                m_status = EditStatus_Invalid;
                
                pMainEditUI->m_pScrollView->setTouchEnabled(true);
                
                Building *pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                PlayerScene::getSingleton().onTouchEnded(worldPoint.x-m_beginPos.x, worldPoint.y-m_beginPos.y);
                if (!PlayerScene::getSingleton().m_pCurBuilding) {
                    PlayerScene::getSingleton().selectBuilding(*pBuilding);
                }
                else {
                    PlayerScene::getSingleton().showArrow(PlayerScene::getSingleton().m_pCurBuilding->getLogicX(),
                                                          PlayerScene::getSingleton().m_pCurBuilding->getLogicY(),
                                                          PlayerScene::getSingleton().m_pCurBuilding->getLogicWidth());
                }
                
                if (m_count == 0) {
                    pMainEditUI->removeBuildingFromEditList(this);
                }
            }
        } break;
            
        default:
            break;
    }
}