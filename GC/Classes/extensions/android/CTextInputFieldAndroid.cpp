//
//  CTextInputFieldAndroid.cpp
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#include "CTextInputFieldAndroid.h"

#include "CTextInputField.h"
#include "jni/Java_org_cocos2dx_lib_Cocos2dxBitmap.h"
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "platform/android/jni/JniHelper.h"

typedef void (*TextInputFieldCallback)(const char* pText, void* ctx);

static TextInputFieldCallback s_pTextInputFieldCallback = NULL;
static void* s_ctx = NULL;

CTextInputFieldImpl* __createSystemTextInputField(CTextInputField* pTextInputField)
{
    return new CTextInputFieldAndroid(pTextInputField);
}

CTextInputFieldAndroid::CTextInputFieldAndroid(CTextInputField* pTextInputField)
: CTextInputFieldImpl(pTextInputField)
, m_pLabel(NULL)
, m_pLabelPlaceHolder(NULL)
, m_eTextFieldInputMode(kTextFieldInputModeSingleLine)
, m_eTextFieldInputFlag(kTextFieldInputFlagInitialCapsSentence)
, m_eKeyboardReturnType(kKeyboardReturnTypeDefault)
, m_colText(ccWHITE)
, m_colPlaceHolder(ccGRAY)
, m_nMaxLength(-1)
, m_obAnchorPoint(ccp(0, 1.0f))
, m_bInRetinaMode(false)
, m_strText("")
{
    
}

CTextInputFieldAndroid::~CTextInputFieldAndroid()
{
	
}

void CTextInputFieldAndroid::doAnimationWhenKeyboardMove(float duration, float distance)
{ // don't need to be implemented on android platform.
	
}

static const int CC_EDIT_BOX_PADDING = 5;

bool CTextInputFieldAndroid::initWithSize(const CCSize& size)
{
	int fontSize = getFontSizeAccordingHeightJni(size.height-8);
	m_pLabel = CCLabelTTF::create("", "", size.height-8);
	// align the text vertically center
	m_pLabel->setAnchorPoint(ccp(0, 0.5f));
	m_pLabel->setVerticalAlignment(kCCVerticalTextAlignmentTop);
	m_pLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
	m_pLabel->setDimensions(CCSizeMake(size.width-4, size.height-4));
	m_pLabel->setPosition(ccp(CC_EDIT_BOX_PADDING, size.height / 2.0f));
	m_pLabel->setColor(m_colText);
	m_pTextInputField->addChild(m_pLabel);
	
	m_pLabelPlaceHolder = CCLabelTTF::create("", "", size.height-8);
	// align the text vertically center
	m_pLabelPlaceHolder->setAnchorPoint(ccp(0, 0.5f));
	m_pLabelPlaceHolder->setPosition(ccp(CC_EDIT_BOX_PADDING, size.height / 2.0f));
	m_pLabelPlaceHolder->setVisible(false);
	m_pLabelPlaceHolder->setColor(m_colPlaceHolder);
	m_pTextInputField->addChild(m_pLabelPlaceHolder);
    
    m_fontSize = fontSize*0.6;
    m_EditSize = size;
    return true;
}

void CTextInputFieldAndroid::setFont(const char* pFontName, int fontSize)
{
	if(m_pLabel != NULL) {
		m_pLabel->setFontName(pFontName);
		m_pLabel->setFontSize(fontSize);
	}
	
	if(m_pLabelPlaceHolder != NULL) {
		m_pLabelPlaceHolder->setFontName(pFontName);
		m_pLabelPlaceHolder->setFontSize(fontSize);
	}

	m_fontSize = fontSize*0.6;
}

void CTextInputFieldAndroid::setFontColor(const ccColor3B& color)
{
    m_colText = color;
    m_pLabel->setColor(color);
}

void CTextInputFieldAndroid::setPlaceholderFont(const char* pFontName, int fontSize)
{
	if(m_pLabelPlaceHolder != NULL) {
		m_pLabelPlaceHolder->setFontName(pFontName);
		m_pLabelPlaceHolder->setFontSize(fontSize);
	}
}

void CTextInputFieldAndroid::setPlaceholderFontColor(const ccColor3B& color)
{
    m_colPlaceHolder = color;
    m_pLabelPlaceHolder->setColor(color);
}

void CTextInputFieldAndroid::setInputMode(TextFieldInputMode inputMode)
{
    m_eTextFieldInputMode = inputMode;
}

void CTextInputFieldAndroid::setMaxLength(int maxLength)
{
    m_nMaxLength = maxLength;
}

int CTextInputFieldAndroid::getMaxLength()
{
    return m_nMaxLength;
}

void CTextInputFieldAndroid::setInputFlag(TextFieldInputFlag inputFlag)
{
    m_eTextFieldInputFlag = inputFlag;
}

void CTextInputFieldAndroid::setReturnType(KeyboardReturnType returnType)
{
    m_eKeyboardReturnType = returnType;
}

bool CTextInputFieldAndroid::isEditing()
{
    return false;
}

void CTextInputFieldAndroid::setText(const char* pText)
{
	m_pLabel->setVisible(true);
	m_pLabelPlaceHolder->setVisible(true);
    if (pText != NULL)
    {
    		int len = strlen(pText);

		if (getMaxLength() < 0 || len <= getMaxLength()) {
			m_strText = pText;
		}
		else {
			m_strText = m_pTextInputField->splitTextByUTF8(pText, getMaxLength());
		}
        
        if (m_strText.length() > 0)
        {
            m_pLabelPlaceHolder->setVisible(false);
			
            std::string strToShow;
			
            if (kTextFieldInputFlagPassword == m_eTextFieldInputFlag)
            {
                long length = cc_utf8_strlen(m_strText.c_str());
                for (long i = 0; i < length; i++)
                {
                    strToShow.append("\u25CF");
                }
            }
            else
            {
                strToShow = m_strText;
            }
            
			m_pLabel->setString(strToShow.c_str());
            
			// Clip the text width to fit to the text box
			float fMaxWidth = m_EditSize.width - CC_EDIT_BOX_PADDING * 2;
			CCRect clippingRect = m_pLabel->getTextureRect();
			if(clippingRect.size.width > fMaxWidth) {
				clippingRect.size.width = fMaxWidth;
				m_pLabel->setTextureRect(clippingRect);
			}
            
        }
        else
        {
            m_pLabelPlaceHolder->setVisible(true);
            m_pLabel->setString("");
        }
    }
}

const char*  CTextInputFieldAndroid::getText(void)
{
    return m_strText.c_str();
}

void CTextInputFieldAndroid::setPlaceHolder(const char* pText)
{
    if (pText != NULL)
    {
        m_strPlaceHolder = pText;
        if (m_strPlaceHolder.length() > 0 && m_strText.length() == 0)
        {
            m_pLabelPlaceHolder->setVisible(true);
        }
		
        m_pLabelPlaceHolder->setString(m_strPlaceHolder.c_str());
    }
}

static CCPoint convertDesignCoordToScreenCoord(const CCPoint& designCoord, bool bInRetinaMode)
{
    CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
    float viewH = (float)eglView->getFrameSize().height;

    CCPoint visiblePos = ccp(designCoord.x * eglView->getScaleX(), designCoord.y * eglView->getScaleY());
    CCPoint screenGLPos = ccpAdd(visiblePos, eglView->getViewPortRect().origin);

    CCPoint screenPos = CCPointMake(screenGLPos.x, viewH - screenGLPos.y);

    if (bInRetinaMode)
    {
        screenPos.x = screenPos.x / 2.0f;
        screenPos.y = screenPos.y / 2.0f;
    }
    CCLOG("[TextFieldInput] pos x = %f, y = %f", screenGLPos.x, screenGLPos.y);
    return screenPos;
}

void CTextInputFieldAndroid::adjustTextFieldPosition()
{
	CCSize contentSize = m_pTextInputField->getContentSize();
	CCRect rect = CCRectMake(0, m_obAnchorPoint.y*contentSize.height, contentSize.width, contentSize.height);
	rect = CCRectApplyAffineTransform(rect, m_pTextInputField->nodeToWorldTransform());

	CCPoint designCoord = ccp(rect.origin.x, rect.origin.y);
	m_EditPosition = convertDesignCoordToScreenCoord(designCoord, m_bInRetinaMode);
}

void CTextInputFieldAndroid::setPosition(const CCPoint& pos)
{
	m_obPosition = pos;
	this->adjustTextFieldPosition();
}

void CTextInputFieldAndroid::setVisible(bool visible)
{ // don't need to be implemented on android platform.
    
}

void CTextInputFieldAndroid::setContentSize(const CCSize& size)
{
	m_tContentSize = size;
	CCLOG("[Edit text] content size = (%f, %f)", size.width, size.height);

	CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
	CCSize controlSize = CCSizeMake(size.width * eglView->getScaleX(),size.height * eglView->getScaleY());
	
	if (m_bInRetinaMode)
	{
		controlSize.width /= 2.0f;
		controlSize.height /= 2.0f;
	}
	m_EditSize = controlSize;
}

void CTextInputFieldAndroid::setAnchorPoint(const CCPoint& anchorPoint)
{ // don't need to be implemented on android platform.
	
}

void CTextInputFieldAndroid::visit(void)
{ // don't need to be implemented on android platform.
    
}

void CTextInputFieldAndroid::onEnter(void)
{
	adjustTextFieldPosition();
//	const char* pText = getText();
//	if (pText) {
//		setInactiveText(pText);
//	}
}

void CTextInputFieldAndroid::onExit()
{

}

static void textInputFieldCallbackFunc(const char* pText, void* ctx)
{
    CTextInputFieldAndroid* pInputFieldAndroid = (CTextInputFieldAndroid*)ctx;
    pInputFieldAndroid->setText(pText);
	
    if (pInputFieldAndroid->getDelegate() != NULL)
    {
    		pInputFieldAndroid->getDelegate()->textInputFieldTextChanged(pInputFieldAndroid->getTextInputField(), pInputFieldAndroid->getText());
    		pInputFieldAndroid->getDelegate()->textInputFieldEditingDidEnd(pInputFieldAndroid->getTextInputField());
    		pInputFieldAndroid->getDelegate()->textInputFieldReturn(pInputFieldAndroid->getTextInputField());
    }
    
    CTextInputField* pTextInputField = pInputFieldAndroid->getTextInputField();
    if (NULL != pTextInputField && 0 != pTextInputField->getScriptTextInputFieldHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "changed",pTextInputField);
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "ended",pTextInputField);
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "return",pTextInputField);
    }
}

void CTextInputFieldAndroid::openKeyboard()
{
    if (m_pDelegate != NULL)
    {
        m_pDelegate->textInputFieldEditingDidBegin(m_pTextInputField);
    }
    CTextInputField* pTextInputField = this->getTextInputField();
    if (NULL != pTextInputField && 0 != pTextInputField->getScriptTextInputFieldHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "began",pTextInputField);
    }
	
    m_pLabel->setVisible(false);
    	m_pLabelPlaceHolder->setVisible(false);
    showTextFieldInputDialogJNI();

    m_pLabel->setVisible(false);

}

void CTextInputFieldAndroid::showTextFieldInputDialogJNI() {

	s_pTextInputFieldCallback = textInputFieldCallbackFunc;
    s_ctx = (void*)this;

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/mgame/common/AppPlatform", "showTextInputFieldDialog", "(Ljava/lang/String;Ljava/lang/String;IIIIIIIII)V")) {

        jstring stringArg1;
        if (m_strPlaceHolder.length() > 0){
        		stringArg1 = t.env->NewStringUTF(m_strPlaceHolder.c_str());
        }else{
        		stringArg1 = t.env->NewStringUTF("");
        }

        jstring stringArg2;
        if (m_strText.length() > 0) {
        		stringArg2 = t.env->NewStringUTF(m_strText.c_str());
		}else{
			stringArg2 = t.env->NewStringUTF("");
		}

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, (int)m_EditPosition.x, (int)m_EditPosition.y, (int)m_EditSize.width, (int)m_EditSize.height, (int)m_fontSize, m_eTextFieldInputMode, m_eTextFieldInputFlag, m_eKeyboardReturnType, m_nMaxLength);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void CTextInputFieldAndroid::closeKeyboard()
{
	
}

extern "C" {

JNIEXPORT void Java_com_mgame_extensions_TextInputField_nativeSetTextInputFieldResult(JNIEnv * env, jobject obj, jbyteArray text) {
        jsize  size = env->GetArrayLength(text);

        if (size > 0) {
            jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
            char* pBuf = (char*)malloc(size+1);
            if (pBuf != NULL) {
                memcpy(pBuf, data, size);
                pBuf[size] = '\0';
                // pass data to edittext's delegate
                if (s_pTextInputFieldCallback) s_pTextInputFieldCallback(pBuf, s_ctx);
                free(pBuf);
            }
            env->ReleaseByteArrayElements(text, data, 0);
        } else {
            if (s_pTextInputFieldCallback) s_pTextInputFieldCallback("", s_ctx);
        }
    }
}
