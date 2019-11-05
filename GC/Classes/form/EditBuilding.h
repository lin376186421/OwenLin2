//
//  EditBuilding.h
//  GameClient
//
//  Created by Li Chao Jun on 14/10/20.
//
//

#ifndef __GameClient__EditBuilding__
#define __GameClient__EditBuilding__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CUINode.h"
#include "MainEditUI.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

typedef enum
{
    EditStatus_Invalid = 0,
    EditStatus_TouchDown,
    EditStatus_DragInside,
    EditStatus_DragOutside,
    EditStatus_TouchEnd,
    EditStatus_Scroll
} EditStatus;

class EditBuilding : public CUINode
{
public:
    EditBuilding();
    ~ EditBuilding();
    
    bool init(BuildingInfo info, CCSize size);
    static EditBuilding* create(BuildingInfo info, CCSize size);
    
public:
    void menuTouchDownCallback(CCObject* pSender, CCControlEvent event);
        
private:
    CCLabelBMFont *m_levelLabel;
    CCLabelBMFont *m_countLabel;
    
    CCControlButton *m_button;
    
public:
    int m_count;
    
    EditStatus m_status;
    CCPoint m_touchDownPoint;
    
    CCPoint m_beginPos;
    
    bool m_isRelease;
            
public:
    void chgCount(int offect);
    
    void scrollViewDidScroll(CCScrollView* view);
    
private:
    void getMaxSourceColorRect(const char* frameName, CCRect &rect);
    
};

#endif /* defined(__GameClient__EditBuilding__) */
