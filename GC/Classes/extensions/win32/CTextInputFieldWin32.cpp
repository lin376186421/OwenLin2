//
//  CTextInputFieldAndroid.cpp
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#include "CTextInputFieldWin32.h"

#include "CTextInputField.h"

typedef void (*TextInputFieldCallback)(const char* pText, void* ctx);

static TextInputFieldCallback s_pTextInputFieldCallback = NULL;
static void* s_ctx = NULL;

CTextInputFieldImpl* __createSystemTextInputField(CTextInputField* pTextInputField)
{
    return new CTextInputFieldWin32(pTextInputField);
}

CTextInputFieldWin32::CTextInputFieldWin32(CTextInputField* pTextInputField)
: CTextInputFieldImpl(pTextInputField)
, m_pLabel(NULL)
, m_pLabelPlaceHolder(NULL)
, m_eTextFieldInputMode(kTextFieldInputModeMultiLine)
, m_eTextFieldInputFlag(kTextFieldInputFlagInitialCapsAllCharacters)
, m_eKeyboardReturnType(kKeyboardReturnTypeDone)
, m_colText(ccWHITE)
, m_colPlaceHolder(ccGRAY)
, m_nMaxLength(-1)
{
    
}

CTextInputFieldWin32::~CTextInputFieldWin32()
{
	
}

void CTextInputFieldWin32::doAnimationWhenKeyboardMove(float duration, float distance)
{ // don't need to be implemented on android platform.
	
}

static const int CC_EDIT_BOX_PADDING = 5;

bool CTextInputFieldWin32::initWithSize(const CCSize& size)
{
    int fontSize = size.height;//getFontSizeAccordingHeightJni(size.height-12);
    m_pLabel = CCLabelTTF::create("", "", size.height-12);
	// align the text vertically center
    m_pLabel->setDimensions(size);
    m_pLabel->setAnchorPoint(ccp(0, 0.5f));
    m_pLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
    m_pLabel->setPosition(ccp(CC_EDIT_BOX_PADDING, size.height / 2.0f));
    m_pLabel->setColor(m_colText);
    m_pTextInputField->addChild(m_pLabel);
	
    m_pLabelPlaceHolder = CCLabelTTF::create("", "", size.height-12);
	// align the text vertically center
    m_pLabelPlaceHolder->setAnchorPoint(ccp(0, 0.5f));
    m_pLabelPlaceHolder->setPosition(ccp(CC_EDIT_BOX_PADDING, size.height / 2.0f));
    m_pLabelPlaceHolder->setVisible(false);
    m_pLabelPlaceHolder->setColor(m_colPlaceHolder);
    m_pTextInputField->addChild(m_pLabelPlaceHolder);
    
    m_EditSize = size;
    return true;
}

void CTextInputFieldWin32::setFont(const char* pFontName, int fontSize)
{
	if(m_pLabel != NULL) {
		m_pLabel->setFontName(pFontName);
		m_pLabel->setFontSize(fontSize);
	}
	
	if(m_pLabelPlaceHolder != NULL) {
		m_pLabelPlaceHolder->setFontName(pFontName);
		m_pLabelPlaceHolder->setFontSize(fontSize);
	}
}

void CTextInputFieldWin32::setFontColor(const ccColor3B& color)
{
    m_colText = color;
    m_pLabel->setColor(color);
}

void CTextInputFieldWin32::setPlaceholderFont(const char* pFontName, int fontSize)
{
	if(m_pLabelPlaceHolder != NULL) {
		m_pLabelPlaceHolder->setFontName(pFontName);
		m_pLabelPlaceHolder->setFontSize(fontSize);
	}
}

void CTextInputFieldWin32::setPlaceholderFontColor(const ccColor3B& color)
{
    m_colPlaceHolder = color;
    m_pLabelPlaceHolder->setColor(color);
}

void CTextInputFieldWin32::setInputMode(TextFieldInputMode inputMode)
{
    m_eTextFieldInputMode = inputMode;
}

void CTextInputFieldWin32::setMaxLength(int maxLength)
{
    m_nMaxLength = maxLength;
}

int CTextInputFieldWin32::getMaxLength()
{
    return m_nMaxLength;
}

void CTextInputFieldWin32::setInputFlag(TextFieldInputFlag inputFlag)
{
    m_eTextFieldInputFlag = inputFlag;
}

void CTextInputFieldWin32::setReturnType(KeyboardReturnType returnType)
{
    m_eKeyboardReturnType = returnType;
}

bool CTextInputFieldWin32::isEditing()
{
    return false;
}

void CTextInputFieldWin32::setText(const char* pText)
{
    if (pText != NULL)
    {
        m_strText = pText;
		
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

const char*  CTextInputFieldWin32::getText(void)
{
    return m_strText.c_str();
}

void CTextInputFieldWin32::setPlaceHolder(const char* pText)
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

void CTextInputFieldWin32::setPosition(const CCPoint& pos)
{ // don't need to be implemented on android platform.
	
}

void CTextInputFieldWin32::setVisible(bool visible)
{ // don't need to be implemented on android platform.
    
}

void CTextInputFieldWin32::setContentSize(const CCSize& size)
{ // don't need to be implemented on android platform.
	
}

void CTextInputFieldWin32::setAnchorPoint(const CCPoint& anchorPoint)
{ // don't need to be implemented on android platform.
	
}

void CTextInputFieldWin32::visit(void)
{ // don't need to be implemented on android platform.
    
}

void CTextInputFieldWin32::onEnter(void)
{ // don't need to be implemented on android platform.
    
}

static void textInputFieldCallbackFunc(const char* pText, void* ctx)
{
    //CTextInputFieldAndroid* pInputFieldAndroid = (CTextInputFieldAndroid*)ctx;
    //pInputFieldAndroid->setText(pText);
	
    //if (pInputFieldAndroid->getDelegate() != NULL)
    //{
    //		pInputFieldAndroid->getDelegate()->textInputFieldTextChanged(pInputFieldAndroid->getTextInputField(), pInputFieldAndroid->getText());
    //		pInputFieldAndroid->getDelegate()->textInputFieldEditingDidEnd(pInputFieldAndroid->getTextInputField());
    //		pInputFieldAndroid->getDelegate()->textInputFieldReturn(pInputFieldAndroid->getTextInputField());
    //}
    //
    //CTextInputField* pTextInputField = pInputFieldAndroid->getTextInputField();
    //if (NULL != pTextInputField && 0 != pTextInputField->getScriptTextInputFieldHandler())
    //{
    //    cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
    //    pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "changed",pTextInputField);
    //    pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "ended",pTextInputField);
    //    pEngine->executeEvent(pTextInputField->getScriptTextInputFieldHandler(), "return",pTextInputField);
    //}
}

void CTextInputFieldWin32::openKeyboard()
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
	
//    showTextFieldInputDialogJNI();
	
}

//void CTextInputFieldWin32::showTextFieldInputDialogJNI() {
//
//	s_pTextInputFieldCallback = textInputFieldCallbackFunc;
//    s_ctx = (void*)this;
//
//    JniMethodInfo t;
//    if (JniHelper::getStaticMethodInfo(t, "com/mgame/common/AppPlatform", "showTextInputFieldDialog", "(Ljava/lang/String;Ljava/lang/String;IIIIII)V")) {
//
//
//        jstring stringArg1;
//        if (m_strPlaceHolder.length() > 0){
//        		stringArg1 = t.env->NewStringUTF(m_strPlaceHolder.c_str());
//        }else{
//        		stringArg1 = t.env->NewStringUTF("");
//        }
//
//        jstring stringArg2;
//        if (m_strText.length() > 0) {
//        		stringArg2 = t.env->NewStringUTF(m_strText.c_str());
//		}else{
//			stringArg2 = t.env->NewStringUTF("");
//		}
//
//        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, (int)m_EditSize.width, (int)m_EditSize.height, m_eTextFieldInputMode, m_eTextFieldInputFlag, m_eKeyboardReturnType, m_nMaxLength);
//
//        t.env->DeleteLocalRef(stringArg1);
//        t.env->DeleteLocalRef(stringArg2);
//        t.env->DeleteLocalRef(t.classID);
//    }
//}

void CTextInputFieldWin32::closeKeyboard()
{
	
}

//extern "C" {
//
//JNIEXPORT void Java_com_mgame_extensions_TextInputFieldDialog_nativeSetTextInputFieldResult(JNIEnv * env, jobject obj, jbyteArray text) {
//        jsize  size = env->GetArrayLength(text);
//
//        if (size > 0) {
//            jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
//            char* pBuf = (char*)malloc(size+1);
//            if (pBuf != NULL) {
//                memcpy(pBuf, data, size);
//                pBuf[size] = '\0';
//                // pass data to edittext's delegate
//                if (s_pTextInputFieldCallback) s_pTextInputFieldCallback(pBuf, s_ctx);
//                free(pBuf);
//            }
//            env->ReleaseByteArrayElements(text, data, 0);
//        } else {
//            if (s_pTextInputFieldCallback) s_pTextInputFieldCallback("", s_ctx);
//        }
//    }
//}
