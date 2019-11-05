#include "CCBFileLoader.h"



NS_CC_EXT_BEGIN

#define PROPERTY_CCBFILE "ccbFile"

void CCBFileLoader::onHandlePropTypeCCBFile(CCNode * pNode, CCNode * pParent, const char * pPropertyName, CCNode * pCCBFileNode, CCBReader * pCCBReader) {
    if(strcmp(pPropertyName, PROPERTY_CCBFILE) == 0) {
        ((CCBFile*)pNode)->setCCBFileNode(pCCBFileNode);
    } else {
        CCNodeLoader::onHandlePropTypeCCBFile(pNode, pParent, pPropertyName, pCCBFileNode, pCCBReader);
    }
}

void CCBFileLoader::onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader) {     //-fix by qsun
    CCNodeLoader::onHandlePropTypeInteger(pNode, pParent, pPropertyName, pInteger, pCCBReader);
}

NS_CC_EXT_END
