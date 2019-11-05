//
//  InPurchase.cpp
//  RobinHood
//
//  Created by qsun on 13-4-9.
//
//

#include "InPurchase.h"
#include "CAppPlatform.h"

InPurchase::InPurchase()
{
    
}

static InPurchase* s_pInPurchase = NULL;
InPurchase* InPurchase::sharedInPurchase()
{
    if (s_pInPurchase == NULL) {
        s_pInPurchase = new InPurchase();
    }
    return s_pInPurchase;
}

void InPurchase::completedPurchase(bool result, string info)
{
	CCLOG("------------------ purchase completed ------------------");
    if (pDelegate && mHandler)
    {
        (pDelegate->*mHandler)(productId, result, info);
    }
}

void InPurchase::makePayment(string productId, CCObject *pDelegate, SEL_InPurchaseHandler callback)
{
    this->purchaseType = PURCHASE_PAYMET;
    
    this->productId = productId;
    this->pDelegate = pDelegate;
    this->mHandler = callback;
    CAppPlatform::sharedAppPlatform()->purchasePayment(productId, -1);
}

void InPurchase::makePayment(string productId, int payType, CCObject *pDelegate, SEL_InPurchaseHandler callback)
{
    this->purchaseType = PURCHASE_PAYMET;
    
    this->productId = productId;
    this->pDelegate = pDelegate;
    this->mHandler = callback;
    CAppPlatform::sharedAppPlatform()->purchasePayment(productId, payType);
}

void InPurchase::resumePurchases(CCObject *pDelegate, SEL_InPurchaseHandler callback)
{
    this->purchaseType = PURCHASE_RESUME;
    
    this->pDelegate = pDelegate;
    this->mHandler = callback;
    CAppPlatform::sharedAppPlatform()->purchasesResume();
}
