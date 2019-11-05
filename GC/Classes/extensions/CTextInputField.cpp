//
//  CTextInputField.cpp
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#include "CTextInputField.h"
#include "CTextInputFieldImpl.h"

CTextInputField::CTextInputField(void)
: m_pTextInputFieldImpl(NULL)
, m_pDelegate(NULL)
, m_eTextFieldInputMode(kTextFieldInputModeSingleLine)
, m_eTextFieldInputFlag(kTextFieldInputFlagInitialCapsAllCharacters)
, m_eKeyboardReturnType(kKeyboardReturnTypeDone)
, m_nFontSize(-1)
, m_nPlaceholderFontSize(-1)
, m_colText(ccWHITE)
, m_colPlaceHolder(ccGRAY)
, m_nMaxLength(0)
, m_fAdjustHeight(0.0f)
, m_nScriptTextInputFieldHandler(0)
{
    
}

CTextInputField::~CTextInputField(void)
{
    CC_SAFE_DELETE(m_pTextInputFieldImpl);
    unregisterScriptTextInputFieldHandler();
}


void CTextInputField::touchDownAction(CCObject *sender, CCControlEvent controlEvent)
{
//    setTouchEnabled(false);
    
    m_pTextInputFieldImpl->openKeyboard();
}

CTextInputField* CTextInputField::create(const CCSize& size, CCScale9Sprite* pNormal9SpriteBg, CCScale9Sprite* pPressed9SpriteBg/* = NULL*/, CCScale9Sprite* pDisabled9SpriteBg/* = NULL*/)
{
    CTextInputField* pRet = new CTextInputField();
    
    if (pRet != NULL && pRet->initWithSizeAndBackgroundSprite(size, pNormal9SpriteBg))
    {
        if (pPressed9SpriteBg != NULL)
        {
            pRet->setBackgroundSpriteForState(pPressed9SpriteBg, CCControlStateHighlighted);
        }
        
        if (pDisabled9SpriteBg != NULL)
        {
            pRet->setBackgroundSpriteForState(pDisabled9SpriteBg, CCControlStateDisabled);
        }
        
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

bool CTextInputField::initWithSizeAndBackgroundSprite(const CCSize& size, CCScale9Sprite* pPressed9SpriteBg)
{
    if (CCControlButton::initWithBackgroundSprite(pPressed9SpriteBg))
    {
        m_pTextInputFieldImpl = __createSystemTextInputField(this);
        m_pTextInputFieldImpl->initWithSize(size);
        
        this->setZoomOnTouchDown(false);
        this->setPreferredSize(size);
        this->setPosition(ccp(0, 0));
        this->addTargetWithActionForControlEvent(this, cccontrol_selector(CTextInputField::touchDownAction), CCControlEventTouchUpInside);
        
        return true;
    }
    return false;
}

void CTextInputField::setDelegate(CTextInputFieldDelegate* pDelegate)
{
    m_pDelegate = pDelegate;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setDelegate(pDelegate);
    }
}

CTextInputFieldDelegate* CTextInputField::getDelegate()
{
    return m_pDelegate;
}

void CTextInputField::setText(const char* pText)
{
    if (pText != NULL)
    {
        m_strText = pText;
        if (m_pTextInputFieldImpl != NULL)
        {
            m_pTextInputFieldImpl->setText(pText);
        }
    }
}

const char* CTextInputField::getText(void)
{
    if (m_pTextInputFieldImpl != NULL)
    {
		const char* pText = m_pTextInputFieldImpl->getText();
		if(pText != NULL)
			return pText;
    }
    
    return "";
}

void CTextInputField::setFont(const char* pFontName, int fontSize)
{
    m_strFontName = pFontName;
    m_nFontSize = fontSize;
    if (pFontName != NULL)
    {
        if (m_pTextInputFieldImpl != NULL)
        {
            m_pTextInputFieldImpl->setFont(pFontName, fontSize);
        }
    }
}

void CTextInputField::setFontName(const char* pFontName)
{
    m_strFontName = pFontName;
    if (m_pTextInputFieldImpl != NULL && m_nFontSize != -1)
    {
        m_pTextInputFieldImpl->setFont(pFontName, m_nFontSize);
    }
}

void CTextInputField::setFontSize(int fontSize)
{
    m_nFontSize = fontSize;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setFont(m_strFontName.c_str(), m_nFontSize);
    }
}

void CTextInputField::setFontColor(const ccColor3B& color)
{
    m_colText = color;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setFontColor(color);
    }
}

void CTextInputField::setPlaceholderFont(const char* pFontName, int fontSize)
{
    m_strPlaceholderFontName = pFontName;
    m_nPlaceholderFontSize = fontSize;
    if (pFontName != NULL)
    {
        if (m_pTextInputFieldImpl != NULL)
        {
            m_pTextInputFieldImpl->setPlaceholderFont(pFontName, fontSize);
        }
    }
}

void CTextInputField::setPlaceholderFontName(const char* pFontName)
{
    m_strPlaceholderFontName = pFontName;
    if (m_pTextInputFieldImpl != NULL && m_nPlaceholderFontSize != -1)
    {
        m_pTextInputFieldImpl->setPlaceholderFont(pFontName, m_nFontSize);
    }
}

void CTextInputField::setPlaceholderFontSize(int fontSize)
{
    m_nPlaceholderFontSize = fontSize;
    if (m_pTextInputFieldImpl != NULL && m_strPlaceholderFontName.length() > 0)
    {
        m_pTextInputFieldImpl->setPlaceholderFont(m_strPlaceholderFontName.c_str(), m_nFontSize);
    }
}

void CTextInputField::setPlaceholderFontColor(const ccColor3B& color)
{
    m_colText = color;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setPlaceholderFontColor(color);
    }
}

void CTextInputField::setPlaceHolder(const char* pText)
{
    if (pText != NULL)
    {
        m_strPlaceHolder = pText;
        if (m_pTextInputFieldImpl != NULL)
        {
            m_pTextInputFieldImpl->setPlaceHolder(pText);
        }
    }
}

const char* CTextInputField::getPlaceHolder(void)
{
    return m_strPlaceHolder.c_str();
}

void CTextInputField::setInputMode(TextFieldInputMode inputMode)
{
    m_eTextFieldInputMode = inputMode;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setInputMode(inputMode);
    }
}

TextFieldInputMode CTextInputField::getInputMode()
{
    return m_eTextFieldInputMode;
}

void CTextInputField::setMaxLength(int maxLength)
{
    m_nMaxLength = maxLength;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setMaxLength(maxLength);
    }
}


int CTextInputField::getMaxLength()
{
    return m_nMaxLength;
}

void CTextInputField::setInputFlag(TextFieldInputFlag inputFlag)
{
    m_eTextFieldInputFlag = inputFlag;
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setInputFlag(inputFlag);
    }
}

void CTextInputField::setReturnType(KeyboardReturnType returnType)
{
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setReturnType(returnType);
    }
}

/* override function */
void CTextInputField::setPosition(const CCPoint& pos)
{
    CCControlButton::setPosition(pos);
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setPosition(pos);
    }
}

void CTextInputField::setVisible(bool visible)
{
    CCControlButton::setVisible(visible);
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setVisible(visible);
    }
}

void CTextInputField::setContentSize(const CCSize& size)
{
    CCControlButton::setContentSize(size);
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setContentSize(size);
    }
}

void CTextInputField::setAnchorPoint(const CCPoint& anchorPoint)
{
    CCControlButton::setAnchorPoint(anchorPoint);
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->setAnchorPoint(anchorPoint);
    }
}

void CTextInputField::visit(void)
{
    CCControlButton::visit();
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->visit();
    }
}

void CTextInputField::onEnter(void)
{
    CCControlButton::onEnter();
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->onEnter();
    }
}

void CTextInputField::onExit(void)
{
    CCControlButton::onExit();
    if (m_pTextInputFieldImpl != NULL)
    {
        // remove system edit control
        m_pTextInputFieldImpl->onExit();
    }
}

static CCRect getRect(CCNode * pNode)
{
	CCSize contentSize = pNode->getContentSize();
	CCRect rect = CCRectMake(0, 0, contentSize.width, contentSize.height);
	return CCRectApplyAffineTransform(rect, pNode->nodeToWorldTransform());
}

void CTextInputField::closeKeyboard()
{
    if (m_pTextInputFieldImpl) {
        m_pTextInputFieldImpl->closeKeyboard();
    }
}

void CTextInputField::keyboardWillShow(CCIMEKeyboardNotificationInfo& info)
{
    // CCLOG("CTextInputField::keyboardWillShow");
    CCRect rectTracked = getRect(this);
	// some adjustment for margin between the keyboard and the edit box.
	rectTracked.origin.y -= 4;
    
    // if the keyboard area doesn't intersect with the tracking node area, nothing needs to be done.
    if (!rectTracked.intersectsRect(info.end))
    {
        CCLOG("needn't to adjust view layout.");
        return;
    }
    
    // assume keyboard at the bottom of screen, calculate the vertical adjustment.
    m_fAdjustHeight = info.end.getMaxY() - rectTracked.getMinY();
    // CCLOG("CTextInputField:needAdjustVerticalPosition(%f)", m_fAdjustHeight);
    
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->doAnimationWhenKeyboardMove(info.duration, m_fAdjustHeight);
    }
}

void CTextInputField::keyboardDidShow(CCIMEKeyboardNotificationInfo& info)
{
	
}

void CTextInputField::keyboardWillHide(CCIMEKeyboardNotificationInfo& info)
{
    // CCLOG("CTextInputField::keyboardWillHide");
    if (m_pTextInputFieldImpl != NULL)
    {
        m_pTextInputFieldImpl->doAnimationWhenKeyboardMove(info.duration, -m_fAdjustHeight);
    }
}

void CTextInputField::keyboardDidHide(CCIMEKeyboardNotificationInfo& info)
{
//	this->setTouchEnabled(true);
    
    if (m_pTextInputFieldImpl != NULL) {
        m_pTextInputFieldImpl->closeKeyboard();
    }
}

void CTextInputField::registerScriptTextInputFieldHandler(int handler)
{
    unregisterScriptTextInputFieldHandler();
    m_nScriptTextInputFieldHandler = handler;
}

void CTextInputField::unregisterScriptTextInputFieldHandler(void)
{
    if (0 != m_nScriptTextInputFieldHandler)
    {
        CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(m_nScriptTextInputFieldHandler);
        m_nScriptTextInputFieldHandler = 0;
    }
}
