//
//  CTextInputFieldIos.h
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#ifndef __GameClient__CTextInputFieldIos__
#define __GameClient__CTextInputFieldIos__

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CTextInputField.h"
#include "CTextInputFieldImpl.h"

using namespace cocos2d;
class CTextInputFieldImpl;

@interface UITextInputField : UITextView
{
}

@end


@interface TextInputFieldIos : NSObject <UITextViewDelegate>
{
    UITextInputField* textField_;
    CTextInputFieldImpl* textInputField_;
    BOOL editState_;
    int fontSize_;
    CGRect frame_;
    
    UIScrollView* textView_;
    
    UITapGestureRecognizer *tapGestureRecognizer_;
    
    CCSize contentSize_;
}

@property(nonatomic, retain) UITextView* textField;
@property(nonatomic, readonly, getter = isEditState) BOOL editState;
@property(nonatomic, assign) void* textInputField;

-(id) initWithFrame: (CGRect) frameRect textInputField: (CTextInputFieldImpl*) pTextInputField;
-(void) doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)distance;
-(void) setPosition:(CGPoint) pos;
-(void) setContentSize:(CGSize) size;
-(void) setContentInset;
-(void) visit;
-(void) openKeyboard;
-(void) closeKeyboard;
-(void) resizeToFit;

-(void)viewTapped:(UITapGestureRecognizer*)tapGestureRecognizer;

@end

class CTextInputFieldIos : public CTextInputFieldImpl
{
public:
    CTextInputFieldIos(CTextInputField* pTextInputField);
    virtual ~CTextInputFieldIos();
    
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
    virtual TextFieldInputMode getInputMode();
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
    virtual void onExit(void);
    virtual void doAnimationWhenKeyboardMove(float duration, float distance);
    virtual void openKeyboard();
    virtual void closeKeyboard();
	
	virtual void onEndEditing();
    
public:
	void			initInactiveLabels(const CCSize& size);
	void			setInactiveText(const char* pText);
	void			adjustTextFieldPosition();
    void            placeInactiveLabels();
	
    CCLabelTTF*     m_pLabel;
    CCLabelTTF*     m_pLabelPlaceHolder;
    CCSize          m_tContentSize;
    CCPoint         m_obPosition;
    CCPoint         m_obAnchorPoint;
    TextInputFieldIos* m_systemControl;
    int             m_nMaxTextLength;
    bool            m_bInRetinaMode;
    
    std::string     m_strText;
};

#endif /* defined(__GameClient__CTextInputFieldIos__) */
