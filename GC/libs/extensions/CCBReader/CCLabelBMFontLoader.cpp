#include "CCLabelBMFontLoader.h"



NS_CC_EXT_BEGIN

#define PROPERTY_COLOR "color"
#define PROPERTY_OPACITY "opacity"
#define PROPERTY_BLENDFUNC "blendFunc"
#define PROPERTY_FNTFILE "fntFile"
#define PROPERTY_STRING "string"
#define PROPERTY_INITIAL_STRING "initialString" //-fix by qsun
#define PROPERTY_ALIGNMENT "Alignment" //-fix by qsun
#define PROPERTY_WIDTH "Width" //-fix by qsun

void CCLabelBMFontLoader::onHandlePropTypeColor3(CCNode * pNode, CCNode * pParent, const char * pPropertyName, ccColor3B pCCColor3B, CCBReader * pCCBReader) {
    if(strcmp(pPropertyName, PROPERTY_COLOR) == 0) {
        ((CCLabelBMFont *)pNode)->setColor(pCCColor3B);
    } else {
        CCNodeLoader::onHandlePropTypeColor3(pNode, pParent, pPropertyName, pCCColor3B, pCCBReader);
    }
}

void CCLabelBMFontLoader::onHandlePropTypeByte(CCNode * pNode, CCNode * pParent, const char * pPropertyName, unsigned char pByte, CCBReader * pCCBReader) {
    if(strcmp(pPropertyName, PROPERTY_OPACITY) == 0) {
        ((CCLabelBMFont *)pNode)->setOpacity(pByte);
    } else {
        CCNodeLoader::onHandlePropTypeByte(pNode, pParent, pPropertyName, pByte, pCCBReader);
    }
}

void CCLabelBMFontLoader::onHandlePropTypeBlendFunc(CCNode * pNode, CCNode * pParent, const char * pPropertyName, ccBlendFunc pCCBlendFunc, CCBReader * pCCBReader) {
    if(strcmp(pPropertyName, PROPERTY_BLENDFUNC) == 0) {
        ((CCLabelBMFont *)pNode)->setBlendFunc(pCCBlendFunc);
    } else {
        CCNodeLoader::onHandlePropTypeBlendFunc(pNode, pParent, pPropertyName, pCCBlendFunc, pCCBReader);
    }
}

void CCLabelBMFontLoader::onHandlePropTypeFntFile(CCNode * pNode, CCNode * pParent, const char * pPropertyName, const char* pFntFile, CCBReader * pCCBReader) {
    if(strcmp(pPropertyName, PROPERTY_FNTFILE) == 0) {
        ((CCLabelBMFont *)pNode)->setFntFile(pFntFile);
    } else {
        CCNodeLoader::onHandlePropTypeFntFile(pNode, pParent, pPropertyName, pFntFile, pCCBReader);
    }
}

void CCLabelBMFontLoader::onHandlePropTypeText(CCNode * pNode, CCNode * pParent, const char * pPropertyName, const char* pText, CCBReader * pCCBReader) {
    if(strcmp(pPropertyName, PROPERTY_STRING) == 0) {
        ((CCLabelBMFont *)pNode)->setString(pText);
    }
    else if (strcmp(pPropertyName, PROPERTY_INITIAL_STRING) == 0) {  //-fix by qsun
        ((CCLabelBMFont *)pNode)->setString(pText);
    }
    else {
        CCNodeLoader::onHandlePropTypeText(pNode, pParent, pPropertyName, pText, pCCBReader);
    }
}

void CCLabelBMFontLoader::onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader)  //-fix by qsun
{
    if(strcmp(pPropertyName, PROPERTY_WIDTH) == 0) {
        ((CCLabelBMFont *)pNode)->setWidth(pFloat);
    } else {
        this->onHandlePropTypeFloat(pNode, pParent, pPropertyName, pFloat, pCCBReader);
    }
}

void CCLabelBMFontLoader::onHandlePropTypeIntegerLabeled(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pIntegerLabeled, CCBReader * pCCBReader){ //-fix by qsun
    if(strcmp(pPropertyName, PROPERTY_ALIGNMENT) == 0) {
        ((CCLabelBMFont *)pNode)->setAlignment((CCTextAlignment)pIntegerLabeled);
    } else {
        this->onHandlePropTypeIntegerLabeled(pNode, pParent, pPropertyName, pIntegerLabeled, pCCBReader);
    }
}

NS_CC_EXT_END