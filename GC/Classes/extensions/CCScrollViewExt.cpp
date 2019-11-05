//
//  CCScrollViewExt.cpp
//  GameClient
//
//  Created by qsun on 13-9-18.
//
//

#include "CCScrollViewExt.h"

void CCScrollViewExt::updateScrollView(CCScrollView* pScrollView, int col, int dir, CCPoint space, CCPoint offset)
{
    CCScrollViewExt::updateScrollView(pScrollView, pScrollView->getContainer(), col, dir, space, offset);
}

void CCScrollViewExt::updateScrollView(CCScrollView* pScrollView, CCNode* pContainerLayer, int col, int dir, CCPoint space, CCPoint offset)
{
    if (pContainerLayer) {
        
        int count = pContainerLayer->getChildrenCount();
        
        float itemWidth = 0;
        float itemHeight = 0;
        if (count > 0) {
            itemWidth = ((CCNode*)pContainerLayer->getChildren()->objectAtIndex(0))->getContentSize().width;
            itemHeight = ((CCNode*)pContainerLayer->getChildren()->objectAtIndex(0))->getContentSize().height;
        }
        
        int row = (count + col - 1)/col;

        float totalWidth = pScrollView->getViewSize().width;
        float totalHeight = pScrollView->getViewSize().height;
        if (dir == SCROLL_HORIZONTAL) {
            totalWidth = itemWidth * col + (col - 1) * space.x + offset.x*2;
            if (totalWidth < pScrollView->getViewSize().width) {
                
                totalWidth = pScrollView->getViewSize().width;
                pContainerLayer->setContentSize(pScrollView->getViewSize());
            }else{
                pContainerLayer->setContentSize(CCSizeMake(totalWidth, pScrollView->getContentSize().height));
            }
        }
        if (dir == SCROLL_VERTICAL) {
            totalHeight = itemHeight * row + (row - 1) * space.y + offset.y*2;
            if (totalHeight < pScrollView->getViewSize().height) {
                
                totalHeight = pScrollView->getViewSize().height;
                pContainerLayer->setContentSize(pScrollView->getViewSize());
            }else{
                pContainerLayer->setContentSize(CCSizeMake(pScrollView->getViewSize().width, totalHeight));
            }
        }
        
        pScrollView->setContentSize(pContainerLayer->getContentSize());
        pScrollView->setContentOffset(CCPointMake(0, pScrollView->getContentOffset().y));
        
        float offX = offset.x;
        float offY = totalHeight - offset.y - itemHeight;
        
        float posX = offX;
        float posY = offY;
        
        CCArray* childrens = pContainerLayer->getChildren();
        if (count > 0) {
            for (int index = 0; index < childrens->count(); index ++) {
                CCNode* node = (CCNode*)childrens->objectAtIndex(index);
                
                if (dir == SCROLL_VERTICAL) {
                    if (index % col == 0 && index != 0) {
                        posY -= (itemHeight + space.y);
                    }
                    
                    node->setPosition(ccp(posX+(itemWidth+space.x)*(index%col), posY));
                }else if(dir == SCROLL_HORIZONTAL){
                    if (index % row == 0 && index != 0) {
                        posX += (itemWidth + space.x);
                        posY = offY;
                    }else if(index != 0){
                        posY -= (itemHeight + space.y);
                    }
                    
                    node->setPosition(ccp(posX, posY));
                }
            }
        }
    }
}