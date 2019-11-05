//
//  CRectLayer.cpp
//  GameClient
//
//  Created by qsun on 13-12-9.
//
//

#include "CRectLayer.h"

CRectLayer* CRectLayer::create(CCSize size, bool bClipToBounds)
{
    CRectLayer* pRet = new CRectLayer();
    
    if (pRet != NULL && pRet->initWithSize(size, bClipToBounds))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
    
    return pRet;

}

bool CRectLayer::initWithSize(CCSize size, bool bClipToBounds)
{
    CCLayer::init();
    
    this->setContentSize(size);
    
    this->m_tViewSize = size;
    this->m_bClipToBounds = bClipToBounds;
    
    return true;
}

CCRect CRectLayer::getViewRect()
{
    CCPoint screenPos = this->convertToWorldSpace(CCPointZero);
    
    float scaleX = this->getScaleX();
    float scaleY = this->getScaleY();
    
    for (CCNode *p = m_pParent; p != NULL; p = p->getParent()) {
        scaleX *= p->getScaleX();
        scaleY *= p->getScaleY();
    }
    
    // Support negative scaling. Not doing so causes intersectsRect calls
    // (eg: to check if the touch was within the bounds) to return false.
    // Note, CCNode::getScale will assert if X and Y scales are different.
    if(scaleX<0.f) {
        screenPos.x += m_tViewSize.width*scaleX;
        scaleX = -scaleX;
    }
    if(scaleY<0.f) {
        screenPos.y += m_tViewSize.height*scaleY;
        scaleY = -scaleY;
    }
    
    return CCRectMake(screenPos.x, screenPos.y, m_tViewSize.width*scaleX, m_tViewSize.height*scaleY);
}

/**
 * clip this view so that outside of the visible bounds can be hidden.
 */
void CRectLayer::beforeDraw()
{
    if (m_bClipToBounds)
    {
		m_bScissorRestored = false;
        CCRect frame = getViewRect();
        if (CCEGLView::sharedOpenGLView()->isScissorEnabled()) {
            m_bScissorRestored = true;
            m_tParentScissorRect = CCEGLView::sharedOpenGLView()->getScissorRect();
            //set the intersection of m_tParentScissorRect and frame as the new scissor rect
            if (frame.intersectsRect(m_tParentScissorRect)) {
                float x = MAX(frame.origin.x, m_tParentScissorRect.origin.x);
                float y = MAX(frame.origin.y, m_tParentScissorRect.origin.y);
                float xx = MIN(frame.origin.x+frame.size.width, m_tParentScissorRect.origin.x+m_tParentScissorRect.size.width);
                float yy = MIN(frame.origin.y+frame.size.height, m_tParentScissorRect.origin.y+m_tParentScissorRect.size.height);
                CCEGLView::sharedOpenGLView()->setScissorInPoints(x, y, xx-x, yy-y);
            }
        }
        else {
            glEnable(GL_SCISSOR_TEST);
            CCEGLView::sharedOpenGLView()->setScissorInPoints(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
        }
    }
}

/**
 * retract what's done in beforeDraw so that there's no side effect to
 * other nodes.
 */
void CRectLayer::afterDraw()
{
    if (m_bClipToBounds)
    {
        if (m_bScissorRestored) {//restore the parent's scissor rect
            CCEGLView::sharedOpenGLView()->setScissorInPoints(m_tParentScissorRect.origin.x, m_tParentScissorRect.origin.y, m_tParentScissorRect.size.width, m_tParentScissorRect.size.height);
        }
        else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void CRectLayer::visit()
{
	// quick return if not visible
	if (!isVisible())
    {
		return;
    }
    
	kmGLPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }
    
	this->transform();
    this->beforeDraw();
    
	if(m_pChildren)
    {
		ccArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode *child =  (CCNode*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode* child = (CCNode*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }
    
    this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }
    
	kmGLPopMatrix();
}
