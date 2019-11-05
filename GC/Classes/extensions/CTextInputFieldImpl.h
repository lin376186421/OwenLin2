//
//  CTextInputFieldImpl.h
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#ifndef GameClient_CTextInputFieldImpl_h
#define GameClient_CTextInputFieldImpl_h

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CTextInputField.h"

class CTextInputFieldImpl
{
public:
    CTextInputFieldImpl(CTextInputField* pTextInputField) : m_pDelegate(NULL),m_pTextInputField(pTextInputField) {}
    virtual ~CTextInputFieldImpl() {}
    
    virtual bool initWithSize(const CCSize& size) = 0;
    virtual void setFont(const char* pFontName, int fontSize) = 0;
    virtual void setFontColor(const ccColor3B& color) = 0;
    virtual void setPlaceholderFont(const char* pFontName, int fontSize) = 0;
    virtual void setPlaceholderFontColor(const ccColor3B& color) = 0;
    virtual void setInputMode(TextFieldInputMode inputMode) = 0;
    virtual void setInputFlag(TextFieldInputFlag inputFlag) = 0;
    virtual void setMaxLength(int maxLength) = 0;
    virtual int  getMaxLength() = 0;
    virtual void setReturnType(KeyboardReturnType returnType) = 0;
    virtual bool isEditing() = 0;
    
    virtual void setText(const char* pText) = 0;
    virtual const char* getText(void) = 0;
    virtual void setPlaceHolder(const char* pText) = 0;
    virtual void doAnimationWhenKeyboardMove(float duration, float distance) = 0;
    
    virtual void openKeyboard() = 0;
    virtual void closeKeyboard() = 0;
    
    virtual void setPosition(const CCPoint& pos) = 0;
    virtual void setVisible(bool visible) = 0;
    virtual void setContentSize(const CCSize& size) = 0;
	virtual void setAnchorPoint(const CCPoint& anchorPoint) = 0;
    virtual void visit(void) = 0;
    virtual void onEnter(void) = 0;
    virtual void onExit(void) = 0;
    
    void setDelegate(CTextInputFieldDelegate* pDelegate) { m_pDelegate = pDelegate; };
    CTextInputFieldDelegate* getDelegate() { return m_pDelegate; };
    CTextInputField* getTextInputField() { return m_pTextInputField; };

protected:
    CTextInputFieldDelegate* m_pDelegate;
    CTextInputField* m_pTextInputField;
};

// This method must be implemented at each subclass of CCEditBoxImpl.
extern CTextInputFieldImpl* __createSystemTextInputField(CTextInputField* pTextInputField);

#endif
