//
//  CTextInputField.h
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#ifndef __GameClient__CTextInputField__
#define __GameClient__CTextInputField__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

/**
 * \brief The TextFieldInputMode defines the type of text that the user is allowed
 * to enter.
 */
enum TextFieldInputMode
{
    /**
     * The user is allowed to enter any text, including line breaks.
     */
    kTextFieldInputModeAny = 0,
    
    /**
     * The user is allowed to enter an e-mail address.
     */
    kTextFieldInputModeEmailAddr,
    
    /**
     * The user is allowed to enter an integer value.
     */
    kTextFieldInputModeNumeric,
    
    /**
     * The user is allowed to enter a phone number.
     */
    kTextFieldInputModePhoneNumber,
    
    /**
     * The user is allowed to enter a URL.
     */
    kTextFieldInputModeUrl,
    
    /**
     * The user is allowed to enter a real number value.
     * This extends kTextFieldInputModeNumeric by allowing a decimal point.
     */
    kTextFieldInputModeDecimal,
    
    /**
     * The user is allowed to enter any text, except for line breaks.
     */
    kTextFieldInputModeSingleLine,
    
    /**
     * The user is allowed to enter any text
     * multi line mode
     */
    kTextFieldInputModeMultiLine
};

/**
 * \brief The TextFieldInputFlag defines how the input text is displayed/formatted.
 */
enum TextFieldInputFlag
{
    /**
     * Indicates that the text entered is confidential data that should be
     * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
     */
    kTextFieldInputFlagPassword = 0,
    
    /**
     * Indicates that the text entered is sensitive data that the
     * implementation must never store into a dictionary or table for use
     * in predictive, auto-completing, or other accelerated input schemes.
     * A credit card number is an example of sensitive data.
     */
    kTextFieldInputFlagSensitive,
    
    /**
     * This flag is a hint to the implementation that during text editing,
     * the initial letter of each word should be capitalized.
     */
    kTextFieldInputFlagInitialCapsWord,
    
    /**
     * This flag is a hint to the implementation that during text editing,
     * the initial letter of each sentence should be capitalized.
     */
    kTextFieldInputFlagInitialCapsSentence,
    
    /**
     * Capitalize all characters automatically.
     */
    kTextFieldInputFlagInitialCapsAllCharacters
    
};

class CTextInputField;
class CTextInputFieldImpl;

class CTextInputFieldDelegate
{
public:
    virtual ~CTextInputFieldDelegate() {};
    
    /**
     * This method is called when an edit box gains focus after keyboard is shown.
     * @param TextInputField The edit box object that generated the event.
     */
    virtual void textInputFieldEditingDidBegin(CTextInputField* TextInputField) {};
    
    
    /**
     * This method is called when an edit box loses focus after keyboard is hidden.
     * @param TextInputField The edit box object that generated the event.
     */
    virtual void textInputFieldEditingDidEnd(CTextInputField* TextInputField) {};
    
    /**
     * This method is called when the edit box text was changed.
     * @param TextInputField The edit box object that generated the event.
     * @param text The new text.
     */
    virtual void textInputFieldTextChanged(CTextInputField* TextInputField, const std::string& text) {};
    
    /**
     * This method is called when the return button was pressed or the outside area of keyboard was touched.
     * @param TextInputField The edit box object that generated the event.
     */
    virtual void textInputFieldReturn(CTextInputField* TextInputField) = 0;
    
};

class CTextInputField
: public CCControlButton
, public CCIMEDelegate
{
public:
    /**
     * Constructor.
     */
    CTextInputField(void);
    
    /**
     * Destructor.
     */
    virtual ~CTextInputField(void);
    
    /**
     * create a edit box with size.
     * @return An autorelease pointer of CTextInputField, you don't need to release it only if you retain it again.
     */
    static CTextInputField* create(const CCSize& size, CCScale9Sprite* pNormal9SpriteBg, CCScale9Sprite* pPressed9SpriteBg = NULL, CCScale9Sprite* pDisabled9SpriteBg = NULL);
    
    /**
     * Init edit box with specified size. This method should be invoked right after constructor.
     * @param size The size of edit box.
     */
    bool initWithSizeAndBackgroundSprite(const CCSize& size, CCScale9Sprite* pNormal9SpriteBg);
    
    /**
     * Gets/Sets the delegate for edit box.
     */
    void setDelegate(CTextInputFieldDelegate* pDelegate);
    CTextInputFieldDelegate* getDelegate();
    /**
     * Registers a script function that will be called for TextInputField events.
     *
     * This handler will be removed automatically after onExit() called.
     * @code
     * -- lua sample
     * local function TextInputFieldEventHandler(eventType)
     *     if eventType == "began" then
     *         -- triggered when an edit box gains focus after keyboard is shown
     *     elseif eventType == "ended" then
     *         -- triggered when an edit box loses focus after keyboard is hidden.
     *     elseif eventType == "changed" then
     *         -- triggered when the edit box text was changed.
     *     elseif eventType == "return" then
     *         -- triggered when the return button was pressed or the outside area of keyboard was touched.
     *     end
     * end
     *
     * local TextInputField = CTextInputField:create(CCSize(...), CCScale9Sprite:create(...))
     * TextInputField = registerScriptTextInputFieldHandler(TextInputFieldEventHandler)
     * @endcode
     *
     * @param handler A number that indicates a lua function.
     */
    void registerScriptTextInputFieldHandler(int handler);
    
    /**
     * Unregisters a script function that will be called for TextInputField events.
     */
    void unregisterScriptTextInputFieldHandler(void);
    /**
     * get a script Handler
     */
    int  getScriptTextInputFieldHandler(void){ return m_nScriptTextInputFieldHandler ;}
    /**
     * Set the text entered in the edit box.
     * @param pText The given text.
     */
    void setText(const char* pText);
    
    /**
     * Get the text entered in the edit box.
     * @return The text entered in the edit box.
     */
    const char* getText(void);
	
	/**
	 * Set the font.
	 * @param pFontName The font name.
	 * @param fontSize The font size.
	 */
	void setFont(const char* pFontName, int fontSize);
    
	/**
	 * Set the font name.
	 * @param pFontName The font name.
	 */
	void setFontName(const char* pFontName);
    
    /**
	 * Set the font size.
	 * @param fontSize The font size.
	 */
	void setFontSize(int fontSize);
    
    /**
     * Set the font color of the widget's text.
     */
    void setFontColor(const ccColor3B& color);
    
	/**
	 * Set the placeholder's font.
	 * @param pFontName The font name.
	 * @param fontSize The font size.
	 */
	void setPlaceholderFont(const char* pFontName, int fontSize);
    
    /**
	 * Set the placeholder's font name.
	 * @param pFontName The font name.
	 */
	void setPlaceholderFontName(const char* pFontName);
    
    /**
	 * Set the placeholder's font size.
	 * @param fontSize The font size.
	 */
	void setPlaceholderFontSize(int fontSize);
    
    /**
     * Set the font color of the placeholder text when the edit box is empty.
     * Not supported on IOS.
     */
    void setPlaceholderFontColor(const ccColor3B& color);
    
    /**
     * Set a text in the edit box that acts as a placeholder when an
     * edit box is empty.
     * @param pText The given text.
     */
    void setPlaceHolder(const char* pText);
    
    /**
     * Get a text in the edit box that acts as a placeholder when an
     * edit box is empty.
     */
    const char* getPlaceHolder(void);
    
    /**
     * Set the input mode of the edit box.
     * @param inputMode One of the TextInputFieldInputMode constants.
     */
    void setInputMode(TextFieldInputMode inputMode);
    
    TextFieldInputMode getInputMode();
    
    /**
     * Sets the maximum input length of the edit box.
     * Setting this value enables multiline input mode by default.
     * Available on Android, iOS and Windows Phone.
     *
     * @param maxLength The maximum length.
     */
    void setMaxLength(int maxLength);
    
    /**
     * Gets the maximum input length of the edit box.
     *
     * @return Maximum input length.
     */
    int getMaxLength();
    
    /**
     * Set the input flags that are to be applied to the edit box.
     * @param inputFlag One of the TextInputFieldInputFlag constants.
     */
    void setInputFlag(TextFieldInputFlag inputFlag);
    
    /**
     * Set the return type that are to be applied to the edit box.
     * @param returnType One of the CCKeyboardReturnType constants.
     */
    void setReturnType(KeyboardReturnType returnType);
    
    /* override functions */
    virtual void setPosition(const CCPoint& pos);
    virtual void setVisible(bool visible);
    virtual void setContentSize(const CCSize& size);
	virtual void setAnchorPoint(const CCPoint& anchorPoint);
    virtual void visit(void);
	virtual void onEnter(void);
    virtual void onExit(void);
    virtual void closeKeyboard();
    virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info);
    
    /* callback funtions */
    void touchDownAction(CCObject *sender, CCControlEvent controlEvent);
    
    std::string splitTextByUTF8(const char* pText, int len)
    {
        std::string text = "";
        int textlen = strlen(pText);
        
        if (textlen < len) {
            text = pText;
            return text;
        }
        
        int pos = 0;
        int idx = 0;
        while (pos < len) {
            
            if ((unsigned char)pText[idx] >= 240) //11110xxx
            {
                if (pos + 2 <= len) { //4 bit
                    idx += 4;
                    pos += 2;
                }
                else {
                    break;
                }
            }
            else if ((unsigned char)pText[idx] >= 224) //1110xxxx
            {
                if (pos + 2 <= len) { //3 bit
                    idx += 3;
                    pos += 2;
                }
                else {
                    break;
                }
            }
            else if ((unsigned char)pText[idx] >= 192) //110xxxxx
            {
                if (pos + 2 <= len) { //2 bit
                    idx += 2;
                    pos += 2;
                }
                else {
                    break;
                }
            }
            else {
                if (pos + 1 <= len) {
                    idx += 1;
                    pos += 1;
                }
            }
        }
        
        text = std::string(pText, idx);
        return text;
    }
    
public:
    CTextInputFieldDelegate*  m_pDelegate;
    
    CTextInputFieldImpl* m_pTextInputFieldImpl;
    
    TextFieldInputMode    m_eTextFieldInputMode;
    TextFieldInputFlag    m_eTextFieldInputFlag;
    KeyboardReturnType    m_eKeyboardReturnType;
    
    std::string m_strText;
    std::string m_strPlaceHolder;
    
    std::string m_strFontName;
    std::string m_strPlaceholderFontName;
    
    int m_nFontSize;
    int m_nPlaceholderFontSize;
    
    ccColor3B m_colText;
    ccColor3B m_colPlaceHolder;
    
    int   m_nMaxLength;
    float m_fAdjustHeight;
    int   m_nScriptTextInputFieldHandler;
};

#endif /* defined(__GameClient__CTextInputField__) */
