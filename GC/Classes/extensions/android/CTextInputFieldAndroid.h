//
//  CTextInputFieldAndroid.h
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#ifndef __GameClient__CTextInputFieldAndroid__
#define __GameClient__CTextInputFieldAndroid__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CTextInputField.h"
#include "CTextInputFieldImpl.h"

class CTextInputFieldAndroid : public CTextInputFieldImpl
{
public:
    CTextInputFieldAndroid(CTextInputField* pTextInputField);
    virtual ~CTextInputFieldAndroid();
    
    virtual bool initWithSize(const CCSize& size);
    virtual void setFont(const char* pFontName, int fontSize);
    virtual void setFontColor(const ccColor3B& color);
    virtual void setPlaceholderFont(const char* pFontName, int fontSize);
    virtual void setPlaceholderFontColor(const ccColor3B& color);
    virtual void setInputMode(TextFieldInputMode inputMode);
    virtual void setInputFlag(TextFieldInputFlag inputFlag);
    virtual void setMaxLength(int maxLength);
    virtual int  getMaxLength();
    virtual void setReturnType(KeyboardReturnType returnType);
    virtual bool isEditing();
    
    virtual void setText(const char* pText);
    virtual const char* getText(void);
    virtual void setPlaceHolder(const char* pText);
    virtual void setPosition(const CCPoint& pos);
    virtual void setVisible(bool visible);
    virtual void setContentSize(const CCSize& size);
	virtual void setAnchorPoint(const CCPoint& anchorPoint);
    virtual void visit(void);
	virtual void onEnter(void);
	virtual void onExit();
    virtual void doAnimationWhenKeyboardMove(float duration, float distance);
    virtual void openKeyboard();
    virtual void closeKeyboard();
    
    void showTextFieldInputDialogJNI();

private:

    void adjustTextFieldPosition();

    CCLabelTTF* m_pLabel;
    CCLabelTTF* m_pLabelPlaceHolder;
    
    TextFieldInputMode  m_eTextFieldInputMode;
    TextFieldInputFlag  m_eTextFieldInputFlag;
    KeyboardReturnType  m_eKeyboardReturnType;
    
    std::string m_strText;
    std::string m_strPlaceHolder;
    CCSize m_tContentSize;
    CCPoint m_obPosition;
    CCPoint m_obAnchorPoint;
    ccColor3B m_colText;
    ccColor3B m_colPlaceHolder;
    
    float m_fontSize;

    int  m_nMaxLength;
    CCPoint m_EditPosition;
    CCSize m_EditSize;
    bool m_bInRetinaMode;
};

#endif /* defined(__GameClient__CTextInputFieldAndroid__) */
