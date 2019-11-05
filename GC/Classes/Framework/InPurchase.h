//
//  InPurchase.h
//  RobinHood
//
//  Created by qsun on 13-4-9.
//
//

#ifndef __platform__InPurchase__
#define __platform__InPurchase__

#include <iostream>
#include "AppConfig.h"
#include "cocos2d.h"

using namespace std;
using namespace cocos2d;

typedef void (CCObject::*SEL_InPurchaseHandler)(string, bool, string);
#define purchase_callback(_SELECTOR) (SEL_InPurchaseHandler)(&_SELECTOR)

#define PURCHASE_VIEW 0

enum PurchaseType{
    
    PURCHASE_PAYMET = 0,
    PURCHASE_RESUME = 1
};

class InPurchase{
public:
    
    InPurchase();
    ~InPurchase();
    
    static InPurchase* sharedInPurchase();
    void completedPurchase(bool result, string info);
    
    void makePayment(string productId, CCObject* pDelegate, SEL_InPurchaseHandler callback);
    void makePayment(string productId, int payType, CCObject *pDelegate, SEL_InPurchaseHandler callback);
    void resumePurchases(CCObject *pDelegate, SEL_InPurchaseHandler callback);
    
    string productId;
    CCObject* pDelegate;
    SEL_InPurchaseHandler mHandler;
    
    int purchaseType;
};

#endif /* defined(__platform__InPurchase__) */
