//
//  CTextInputFieldIos.cpp
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#include "CTextInputFieldIos.h"

#define kLabelZOrder  9999

#import "CTextInputFieldIos.h"
#import "EAGLView.h"
#import "CRectLayer.h"

#define getTextInputFieldIos() ((CTextInputFieldIos*)textInputField_)

static const int TEXT_INPUT_FILED_PADDING = 5;

@implementation UITextInputField
@end

@implementation TextInputFieldIos

@synthesize textField = textField_;
@synthesize editState = editState_;
@synthesize textInputField = textInputField_;

- (void)dealloc
{
    [[EAGLView sharedEGLView] removeGestureRecognizer:tapGestureRecognizer_];
    [tapGestureRecognizer_ release];
    
    self.textField = NULL;
    [super dealloc];
}

-(id) initWithFrame: (CGRect) frameRect textInputField: (CTextInputFieldImpl*) pTextInputField
{
    self = [super init];
    
    do
    {
        if (self == nil) break;
                
        editState_ = NO;
        fontSize_ = frameRect.size.height*2/3;
        frame_ = frameRect;
        
        self.textField = [[[UITextInputField alloc] initWithFrame: frameRect] autorelease];
        if (!textField_) break;
        [textField_ setTextColor:[UIColor whiteColor]];
        textField_.font = [UIFont systemFontOfSize:fontSize_];
//		textField_.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
        textField_.backgroundColor = [UIColor clearColor];
//        textField_.backgroundColor = [UIColor colorWithRed:255 green:0 blue:0 alpha:80];
        textField_.returnKeyType = UIReturnKeyDefault;
//        textField_.contentInset = UIEdgeInsetsMake(0, 0, frameRect.size.height, frameRect.size.width);
        textField_.keyboardType = UIKeyboardTypeDefault;
        textField_.scrollEnabled = true;
        textField_.showsVerticalScrollIndicator = false;
        textField_.showsHorizontalScrollIndicator = false;
        textField_.bounces = false;
        textField_.delegate = self;
        textField_.hidden = FALSE;
        
        CGRect frame = CGRectMake(0, 0, frameRect.size.width, frameRect.size.height);
        textView_ = [[UIScrollView alloc] initWithFrame: frame];
        textView_.scrollEnabled = true;
        textView_.showsVerticalScrollIndicator = false;
        textView_.showsHorizontalScrollIndicator = false;
        textView_.bounces = false;
        textView_.backgroundColor = [UIColor clearColor];
//        textView_.backgroundColor = [UIColor colorWithRed:0 green:255 blue:0 alpha:180];
        
        [textView_ addSubview:textField_];
        
        self.textInputField = pTextInputField;
        
        tapGestureRecognizer_ = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapped:)];
        
        return self;
    }while(0);
    
    return nil;
}

-(void) doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)distance
{
    id eglView = [EAGLView sharedEGLView];
    [eglView doAnimationWhenKeyboardMoveWithDuration:duration distance:distance];
}

-(void) resizeToFit
{
    if (getTextInputFieldIos()->getInputMode() == kTextFieldInputModeMultiLine)
    {
        // multiline
    }
    else
    {
        CGSize size = [textField_.text sizeWithFont:textField_.font];
        
        size = CGSizeMake(size.width + 50, frame_.size.height);
        
        if (size.width < frame_.size.width) {
            size.width = frame_.size.width;
        }
        
        [textField_ setFrame:CGRectMake(0, 0, size.width, size.height)];
        [textView_ setContentSize:size];
        
        //adjust focus
        if (textField_.selectedTextRange) {
            CGPoint pos = [textField_ caretRectForPosition:textField_.selectedTextRange.end].origin;
            if (pos.x > textView_.contentOffset.x + frame_.size.width) {
                [textView_ setContentOffset:CGPointMake(pos.x-frame_.size.width+10, textView_.contentOffset.y)];
            }
            else if (pos.x < textView_.contentOffset.x) {
                [textView_ setContentOffset:CGPointMake(pos.x, textView_.contentOffset.y)];
            }
        }
    }
}

-(void) setPosition:(CGPoint) pos
{
    CGRect frame = [textView_ frame];
    frame.origin = pos;
    
    [textView_ setFrame:frame];
}

-(void) setContentSize:(CGSize) size
{
    CGRect frame = [textView_ frame];
    frame.size = size;
    
    [textView_ setContentSize:size];
    [textView_ setFrame:frame];
    
    [self resizeToFit];
}

-(void) visit
{
}

-(void) openKeyboard
{
    [[EAGLView sharedEGLView] addSubview:textView_];
    [textField_ becomeFirstResponder];
    
    [[EAGLView sharedEGLView] addGestureRecognizer:tapGestureRecognizer_]; 
}

-(void) closeKeyboard
{
    [textField_ resignFirstResponder];
    [textView_ removeFromSuperview];
    
    [[EAGLView sharedEGLView] removeGestureRecognizer:tapGestureRecognizer_];
}

-(void)viewTapped:(UITapGestureRecognizer*)tapGestureRecognizer{
    
    [self closeKeyboard];
    
    [[EAGLView sharedEGLView] removeGestureRecognizer:tapGestureRecognizer_];
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text{
    
    if (getTextInputFieldIos()->getInputMode() != kTextFieldInputModeMultiLine) {
        if ([text isEqualToString:@"\n"]) {
            [self closeKeyboard];
            return NO;
        }
    }
    
    if (getTextInputFieldIos()->getMaxLength() < 0)
    {
        return YES;
    }
    
    int len = 0;
    if (getTextInputFieldIos()->getInputMode() == kTextFieldInputModeMultiLine) { //多行输入去除 \n
        int fontw = fontSize_*0.2;
        
        if ([text isEqualToString:@"\n"]) {
            len += frame_.size.width/fontw;
        }
        
        for (int i = 0; i < textField_.text.length; i++) {
            char ch = [textField_.text characterAtIndex:i];
            
            if (ch == '\n') {
                len += frame_.size.width/fontw;
            }
        }
    }
    
    NSUInteger oldLength = [textField_.text length];
    NSUInteger replacementLength = [text length];
    NSUInteger rangeLength = range.length;
    
    NSUInteger newLength = oldLength - rangeLength + replacementLength + len;
    
    return newLength <= getTextInputFieldIos()->getMaxLength();
}

- (void)textViewDidChangeSelection:(UITextView *)textView
{

}

-(void)animationSelector
{
    id eglView = [EAGLView sharedEGLView];
    [eglView doAnimationWhenAnotherEditBeClicked];
}

- (BOOL)textViewShouldBeginEditing:(UITextView *)textView       // return NO to disallow editing.
{
    CCLOG("textFieldShouldBeginEditing...");
    editState_ = YES;
    id eglView = [EAGLView sharedEGLView];
    if ([eglView isKeyboardShown])
    {
        [self performSelector:@selector(animationSelector) withObject:nil afterDelay:0.0f];
    }
    CTextInputFieldDelegate* pDelegate = getTextInputFieldIos()->getDelegate();
    if (pDelegate != NULL)
    {
        pDelegate->textInputFieldEditingDidBegin(getTextInputFieldIos()->getTextInputField());
    }
    
    CTextInputField*  pTextInputField= getTextInputFieldIos()->getTextInputField();
    if (NULL != pTextInputField && 0 != pTextInputField->getScriptTextInputFieldHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "began", pTextInputField);
    }
    return YES;
}

- (BOOL)textViewShouldEndEditing:(UITextView *)textView
{
    CCLOG("textFieldShouldEndEditing...");
    editState_ = NO;
    getTextInputFieldIos()->setText(getTextInputFieldIos()->getText());
    
    CTextInputFieldDelegate* pDelegate = getTextInputFieldIos()->getDelegate();
    if (pDelegate != NULL)
    {
        pDelegate->textInputFieldEditingDidEnd(getTextInputFieldIos()->getTextInputField());
        pDelegate->textInputFieldReturn(getTextInputFieldIos()->getTextInputField());
    }
    
    CTextInputField*  pTextInputField= getTextInputFieldIos()->getTextInputField();
    if (NULL != pTextInputField && 0 != pTextInputField->getScriptTextInputFieldHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "ended",pTextInputField);
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "return",pTextInputField);
    }
	
	if(textInputField_ != nil)
	{
		getTextInputFieldIos()->onEndEditing();
	}
        
    return YES;
}

- (void)textViewDidBeginEditing:(UITextView *)textView
{
}

- (void)textViewDidEndEditing:(UITextView *)textView
{
    
}

/**
 * Called each time when the text field's text has changed.
 */
- (void)textViewDidChange:(UITextView *)textView
{
    CTextInputField* pTextInputField = getTextInputFieldIos()->getTextInputField();
    
    if (textField_.markedTextRange == nil) {
        pTextInputField->setText([textField_.text UTF8String]);
    }
    
    // NSLog(@"text is %@", self.textField.text);
    CTextInputFieldDelegate* pDelegate = getTextInputFieldIos()->getDelegate();
    if (pDelegate != NULL)
    {
        pDelegate->textInputFieldTextChanged(getTextInputFieldIos()->getTextInputField(), getTextInputFieldIos()->getText());
    }
    
    if (NULL != pTextInputField && 0 != pTextInputField->getScriptTextInputFieldHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "changed", pTextInputField);
    }
    
    [self resizeToFit];
}

@end

CTextInputFieldImpl* __createSystemTextInputField(CTextInputField* pTextInputField)
{
    return new CTextInputFieldIos(pTextInputField);
}

CTextInputFieldIos::CTextInputFieldIos(CTextInputField* pTextInputField)
: CTextInputFieldImpl(pTextInputField)
, m_pLabel(NULL)
, m_pLabelPlaceHolder(NULL)
, m_systemControl(NULL)
, m_obAnchorPoint(ccp(0.5f, 0.5f))
, m_nMaxTextLength(-1)
, m_strText("")
{
    m_bInRetinaMode = [[EAGLView sharedEGLView] contentScaleFactor] == 2.0f ? true : false;
}

CTextInputFieldIos::~CTextInputFieldIos()
{
    [m_systemControl release];
}

void CTextInputFieldIos::doAnimationWhenKeyboardMove(float duration, float distance)
{
    if ([m_systemControl isEditState] || distance < 0.0f)
    {
        [m_systemControl doAnimationWhenKeyboardMoveWithDuration:duration distance:distance];
    }
}

bool CTextInputFieldIos::initWithSize(const CCSize& size)
{
    do
    {
        CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
        CGRect rect = CGRectMake(0, 0, size.width * eglView->getScaleX(), size.height * eglView->getScaleY());
        
        if (m_bInRetinaMode)
        {
            rect.size.width /= 2.0f;
            rect.size.height /= 2.0f;
        }
        
        m_systemControl = [[TextInputFieldIos alloc] initWithFrame:rect textInputField:this];
        if (!m_systemControl) break;
        
		initInactiveLabels(size);
        setContentSize(size);
        		
        return true;
    }while (0);
    
    return false;
}

void CTextInputFieldIos::initInactiveLabels(const CCSize& size)
{
	const char* pDefaultFontName = [[m_systemControl.textField.font fontName] UTF8String];
    
	m_pLabel = CCLabelTTF::create("", "", 0.0f);
    m_pLabel->setAnchorPoint(ccp(0, 0.5f));
    m_pLabel->setColor(ccWHITE);
    m_pLabel->setVerticalAlignment(kCCVerticalTextAlignmentTop);
    m_pLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
    m_pLabel->setDimensions(CCSizeMake(size.width-16, size.height-16));
    m_pLabel->setVisible(false);
    
    CRectLayer* pRectLayer = CRectLayer::create(size);
    pRectLayer->addChild(m_pLabel);
    
    m_pLabelPlaceHolder = CCLabelTTF::create("", "", 0.0f);
	// align the text vertically center
    m_pLabelPlaceHolder->setAnchorPoint(ccp(0, 0.5f));
    m_pLabelPlaceHolder->setColor(ccGRAY);
    pRectLayer->addChild(m_pLabelPlaceHolder);
    
    m_pTextInputField->addChild(pRectLayer, kLabelZOrder);
    
    setFont(pDefaultFontName, size.height*2/3);
    setPlaceholderFont(pDefaultFontName, size.height*2/3);
}

void CTextInputFieldIos::placeInactiveLabels() {
    m_pLabel->setPosition(ccp(TEXT_INPUT_FILED_PADDING, m_tContentSize.height / 2.0f));
    m_pLabelPlaceHolder->setPosition(ccp(TEXT_INPUT_FILED_PADDING, m_tContentSize.height / 2.0f));
}

void CTextInputFieldIos::setInactiveText(const char* pText)
{
	if(m_systemControl.textField.secureTextEntry == YES)
	{
		std::string passwordString;
		for(int i = 0; i < strlen(pText); ++i)
			passwordString.append("\u25CF");
		m_pLabel->setString(passwordString.c_str());
	}
	else
		m_pLabel->setString(getText());
	
	// Clip the text width to fit to the text box
	float fMaxWidth = m_pTextInputField->getContentSize().width - TEXT_INPUT_FILED_PADDING * 2;
	CCRect clippingRect = m_pLabel->getTextureRect();
	if(clippingRect.size.width > fMaxWidth) {
		clippingRect.size.width = fMaxWidth;
		m_pLabel->setTextureRect(clippingRect);
	}
}

void CTextInputFieldIos::setFont(const char* pFontName, int fontSize)
{
    bool isValidFontName = true;
	if(pFontName == NULL || strlen(pFontName) == 0) {
        isValidFontName = false;
    }
    
    float retinaFactor = m_bInRetinaMode ? 2.0f : 1.0f;
	NSString * fntName = [NSString stringWithUTF8String:pFontName];
    float scaleFactor = CCEGLView::sharedOpenGLView()->getScaleX();
    UIFont *textFont = nil;
    if (isValidFontName) {
        textFont = [UIFont fontWithName:fntName size:fontSize * scaleFactor / retinaFactor];
    }
    
    if (!isValidFontName || textFont == nil){
        textFont = [UIFont systemFontOfSize:fontSize * scaleFactor / retinaFactor];
    }
    
	if(textFont != nil) {
		[m_systemControl.textField setFont:textFont];
    }
    
	m_pLabel->setFontName(pFontName);
	m_pLabel->setFontSize(fontSize);
	m_pLabelPlaceHolder->setFontName(pFontName);
	m_pLabelPlaceHolder->setFontSize(fontSize);
}

void CTextInputFieldIos::setFontColor(const ccColor3B& color)
{
    m_systemControl.textField.textColor = [UIColor colorWithRed:color.r / 255.0f green:color.g / 255.0f blue:color.b / 255.0f alpha:1.0f];
	m_pLabel->setColor(color);
}

void CTextInputFieldIos::setPlaceholderFont(const char* pFontName, int fontSize)
{
	// TODO need to be implemented.
}

void CTextInputFieldIos::setPlaceholderFontColor(const ccColor3B& color)
{
	m_pLabelPlaceHolder->setColor(color);
}

void CTextInputFieldIos::setInputMode(TextFieldInputMode inputMode)
{
    switch (inputMode)
    {
        case kTextFieldInputModeEmailAddr:
            m_systemControl.textField.keyboardType = UIKeyboardTypeEmailAddress;
            break;
        case kTextFieldInputModeNumeric:
            m_systemControl.textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
            break;
        case kTextFieldInputModePhoneNumber:
            m_systemControl.textField.keyboardType = UIKeyboardTypePhonePad;
            break;
        case kTextFieldInputModeUrl:
            m_systemControl.textField.keyboardType = UIKeyboardTypeURL;
            break;
        case kTextFieldInputModeDecimal:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDecimalPad;
            break;
        case kTextFieldInputModeSingleLine:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
        case kTextFieldInputModeMultiLine:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
        default:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
    }
}

void CTextInputFieldIos::setMaxLength(int maxLength)
{
    m_nMaxTextLength = maxLength;
}

int CTextInputFieldIos::getMaxLength()
{
    return m_nMaxTextLength;
}

void CTextInputFieldIos::setInputFlag(TextFieldInputFlag inputFlag)
{
    switch (inputFlag)
    {
        case kTextFieldInputFlagPassword:
            m_systemControl.textField.secureTextEntry = YES;
            break;
        case kTextFieldInputFlagInitialCapsWord:
            m_systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeWords;
            break;
        case kTextFieldInputFlagInitialCapsSentence:
            m_systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeSentences;
            break;
        case kTextFieldInputFlagInitialCapsAllCharacters:
            m_systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeAllCharacters;
            break;
        case kTextFieldInputFlagSensitive:
            m_systemControl.textField.autocorrectionType = UITextAutocorrectionTypeNo;
            break;
        default:
            break;
    }
}

void CTextInputFieldIos::setReturnType(KeyboardReturnType returnType)
{
    switch (returnType) {
        case kKeyboardReturnTypeDefault:
            m_systemControl.textField.returnKeyType = UIReturnKeyDefault;
            break;
        case kKeyboardReturnTypeDone:
            m_systemControl.textField.returnKeyType = UIReturnKeyDone;
            break;
        case kKeyboardReturnTypeSend:
            m_systemControl.textField.returnKeyType = UIReturnKeySend;
            break;
        case kKeyboardReturnTypeSearch:
            m_systemControl.textField.returnKeyType = UIReturnKeySearch;
            break;
        case kKeyboardReturnTypeGo:
            m_systemControl.textField.returnKeyType = UIReturnKeyGo;
            break;
        default:
            m_systemControl.textField.returnKeyType = UIReturnKeyDefault;
            break;
    }
}

TextFieldInputMode CTextInputFieldIos::getInputMode()
{
    return m_pTextInputField->getInputMode();
}

bool CTextInputFieldIos::isEditing()
{
    return [m_systemControl isEditState] ? true : false;
}

void CTextInputFieldIos::setText(const char* pText)
{
    if (pText != NULL)
    {
        int len = strlen(pText);

        if (getMaxLength() < 0 || len <= getMaxLength()) {
            m_strText = pText;
        }
        else {
            m_strText = m_pTextInputField->splitTextByUTF8(pText, getMaxLength());
        }
        
        m_systemControl.textField.text = [NSString stringWithUTF8String:m_strText.c_str()];
        setInactiveText(m_strText.c_str());
        
        if(m_systemControl.textField.hidden == YES) {
            
            if(len == 0)
            {
                m_pLabel->setVisible(false);
                m_pLabelPlaceHolder->setVisible(true);
            }
            else
            {
                m_pLabel->setVisible(true);
                m_pLabelPlaceHolder->setVisible(false);
            }
        }
    }
}

const char* CTextInputFieldIos::getText(void)
{
    return [m_systemControl.textField.text UTF8String];
}

void CTextInputFieldIos::setPlaceHolder(const char* pText)
{
//    m_systemControl.textField.placeholder = [NSString stringWithUTF8String:pText];
//	m_pLabelPlaceHolder->setString(pText);
}

static CGPoint convertDesignCoordToScreenCoord(const CCPoint& designCoord, bool bInRetinaMode)
{
    CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
    float viewH = (float)[[EAGLView sharedEGLView] getHeight];
    
    CCPoint visiblePos = ccp(designCoord.x * eglView->getScaleX(), designCoord.y * eglView->getScaleY());
    CCPoint screenGLPos = ccpAdd(visiblePos, eglView->getViewPortRect().origin);
    
    CGPoint screenPos = CGPointMake(screenGLPos.x, viewH - screenGLPos.y);
    
    if (bInRetinaMode)
    {
        screenPos.x = screenPos.x / 2.0f;
        screenPos.y = screenPos.y / 2.0f;
    }
        
    CCLOG("[TextFieldInput] pos x = %f, y = %f", screenGLPos.x, screenGLPos.y);
    return screenPos;
}

void CTextInputFieldIos::setPosition(const CCPoint& pos)
{
	m_obPosition = pos;
	adjustTextFieldPosition();
}

void CTextInputFieldIos::setVisible(bool visible)
{
    //    m_systemControl.textField.hidden = !visible;
}

void CTextInputFieldIos::setContentSize(const CCSize& size)
{
    m_tContentSize = size;
    CCLOG("[Text Input] content size = (%f, %f)", size.width, size.height);
    placeInactiveLabels();
    CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
    CGSize controlSize = CGSizeMake(size.width * eglView->getScaleX(),size.height * eglView->getScaleY());
    
    if (m_bInRetinaMode)
    {
        controlSize.width /= 2.0f;
        controlSize.height /= 2.0f;
    }
    [m_systemControl setContentSize:controlSize];
}

void CTextInputFieldIos::setAnchorPoint(const CCPoint& anchorPoint)
{
    CCLOG("[Text Input] anchor point = (%f, %f)", anchorPoint.x, anchorPoint.y);
	m_obAnchorPoint = anchorPoint;
	setPosition(m_obPosition);
}

void CTextInputFieldIos::visit(void)
{
    
}

void CTextInputFieldIos::onEnter(void)
{
    adjustTextFieldPosition();
    const char* pText = getText();
    if (pText) {
        setInactiveText(pText);
    }
}

void CTextInputFieldIos::onExit(void)
{
    this->closeKeyboard();
}

void CTextInputFieldIos::adjustTextFieldPosition()
{
	CCSize contentSize = m_pTextInputField->getContentSize();
	CCRect rect = CCRectMake(0, 0, contentSize.width, contentSize.height);
    rect = CCRectApplyAffineTransform(rect, m_pTextInputField->nodeToWorldTransform());
    
	CCPoint designCoord = ccp(rect.origin.x, rect.origin.y + rect.size.height);
    [m_systemControl setPosition:convertDesignCoordToScreenCoord(designCoord, m_bInRetinaMode)];
}

void CTextInputFieldIos::openKeyboard()
{
	m_pLabel->setVisible(false);
	m_pLabelPlaceHolder->setVisible(false);
    
	m_systemControl.textField.hidden = NO;
    [m_systemControl openKeyboard];
}

void CTextInputFieldIos::closeKeyboard()
{
    [m_systemControl closeKeyboard];
}

void CTextInputFieldIos::onEndEditing()
{
	m_systemControl.textField.hidden = YES;
	if(strlen(getText()) == 0)
	{
		m_pLabel->setVisible(false);
		m_pLabelPlaceHolder->setVisible(true);
	}
	else
	{
		m_pLabel->setVisible(true);
		m_pLabelPlaceHolder->setVisible(false);
		setInactiveText(getText());
	}
}